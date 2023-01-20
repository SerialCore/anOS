#include <arch/bcm2837/uart0.h>
#include <arch/bcm2837/timer.h>
#include <arch/bcm2837/emmc.h>
#include <arch/bcm2837/gpio.h>
#include <lib/math/basicmath.h>

unsigned long sd_scr[2] = {0}, sd_rca = 0, sd_ocr = 0, sd_err = 0, sd_hv = 0;

// Wait for data or command ready
int _emmc_status(unsigned int mask)
{
    int cnt = 500000;
    while ((*EMMC_STATUS & mask) && !(*EMMC_INTERRUPT & INT_ERROR_MASK) && cnt--) _delay_microseconds(1);
    return (cnt <= 0 || (*EMMC_INTERRUPT & INT_ERROR_MASK)) ? SD_ERROR : SD_OK;
}

// Wait for interrupt
int _emmc_interrupt(unsigned int mask)
{
    unsigned int r, m = mask | INT_ERROR_MASK;
    int cnt = 1000000;
    while (!(*EMMC_INTERRUPT & m) && cnt--) _delay_microseconds(1);
    r = *EMMC_INTERRUPT;
    if (cnt <= 0 || (r & INT_CMD_TIMEOUT) || (r & INT_DATA_TIMEOUT) )
    {
        *EMMC_INTERRUPT = r;
        return SD_TIMEOUT;
    }
    else if (r & INT_ERROR_MASK)
    {
        *EMMC_INTERRUPT = r;
        return SD_ERROR;
    }
    *EMMC_INTERRUPT = mask;
    return 0;
}

// Send a command
int _emmc_command(unsigned int code, unsigned int arg)
{
    int r = 0;
    sd_err = SD_OK;
    if (code & CMD_NEED_APP)
    {
        r = _emmc_command(CMD_APP_CMD | (sd_rca ? CMD_RSPNS_48 : 0) ,sd_rca);
        if (sd_rca && !r)
        {
            _uart0_sendstring("ERROR: failed to send SD APP command\n");
            sd_err = SD_ERROR;
            return 0;
        }
        code &= ~CMD_NEED_APP;
    }
    if (_emmc_status(SR_CMD_INHIBIT))
    {
        _uart0_sendstring("ERROR: EMMC busy\n");
        sd_err = SD_TIMEOUT;
        return 0;
    }
    _uart0_sendstring("EMMC: Sending command ");
    _uart0_sendhex(code);
    _uart0_sendstring(" arg ");
    _uart0_sendhex(arg);
    _uart0_sendstring("\n");
    *EMMC_INTERRUPT = *EMMC_INTERRUPT;
    *EMMC_ARG1 = arg;
    *EMMC_CMDTM = code;
    if (code == CMD_SEND_OP_COND)
        _delay_microseconds(1000);
    else if (code == CMD_SEND_IF_COND || code == CMD_APP_CMD)
        _delay_microseconds(100);
    if ((r = _emmc_interrupt(INT_CMD_DONE)))
    {
        _uart0_sendstring("ERROR: failed to send EMMC command\n");
        sd_err = r;
        return 0;
    }
    r = *EMMC_RESP0;
    if (code == CMD_GO_IDLE || code == CMD_APP_CMD)
        return 0;
    else if (code == (CMD_APP_CMD | CMD_RSPNS_48))
        return r & SR_APP_CMD;
    else if (code == CMD_SEND_OP_COND)
        return r;
    else if (code == CMD_SEND_IF_COND)
        return r == arg ? SD_OK : SD_ERROR;
    else if (code == CMD_ALL_SEND_CID)
    {
        r |= *EMMC_RESP3;
        r |= *EMMC_RESP2;
        r |= *EMMC_RESP1;
        return r;
    }
    else if (code == CMD_SEND_REL_ADDR)
    {
        sd_err = (((r & 0x1fff)) | ((r & 0x2000) << 6) | ((r & 0x4000) << 8) | ((r & 0x8000) << 8)) & CMD_ERRORS_MASK;
        return r & CMD_RCA_MASK;
    }
    return r & CMD_ERRORS_MASK;
    // make gcc happy
    return 0;
}

int _emmc_readblock(unsigned int lba, unsigned char *buffer, unsigned int num)
{
    int r, c = 0, d;
    if (num < 1)
        num = 1;
    _uart0_sendstring("sd_readblock lba ");
    _uart0_sendhex(lba);
    _uart0_sendstring(" num ");
    _uart0_sendhex(num);
    _uart0_sendstring("\n");
    if (_emmc_status(SR_DAT_INHIBIT))
    {
        sd_err = SD_TIMEOUT;
        return 0;
    }
    unsigned int* buf = (unsigned int*)buffer;
    if (sd_scr[0] & SCR_SUPP_CCS)
    {
        if (num > 1 && (sd_scr[0] & SCR_SUPP_SET_BLKCNT))
        {
            _emmc_command(CMD_SET_BLOCKCNT, num);
            if (sd_err)
                return 0;
        }
        *EMMC_BLKSIZECNT = (num << 16) | 512;
        _emmc_command(num == 1 ? CMD_READ_SINGLE : CMD_READ_MULTI, lba);
        if (sd_err)
            return 0;
    }
    else
    {
        *EMMC_BLKSIZECNT = (1 << 16) | 512;
    }
    while (c < num)
    {
        if (!(sd_scr[0] & SCR_SUPP_CCS))
        {
            _emmc_command(CMD_READ_SINGLE, (lba + c) * 512);
            if (sd_err)
                return 0;
        }
        if ((r = _emmc_interrupt(INT_READ_RDY)))
        {
            _uart0_sendstring("\rERROR: Timeout waiting for ready to read\n");
            sd_err = r;
            return 0;
        }
        for (d = 0; d < 128; d++)
            buf[d] = *EMMC_DATA;
        c++;
        buf += 128;
    }
    if (num > 1 && !(sd_scr[0] & SCR_SUPP_SET_BLKCNT) && (sd_scr[0] & SCR_SUPP_CCS))
        _emmc_command(CMD_STOP_TRANS, 0);
    return sd_err != SD_OK || c != num ? 0 : num * 512;
}

int _emmc_writeblock(unsigned char *buffer, unsigned int lba, unsigned int num)
{
    int r, c = 0, d;
    if (num < 1)
        num = 1;
    _uart0_sendstring("sd_writeblock lba ");
    _uart0_sendhex(lba);
    _uart0_sendstring(" num ");
    _uart0_sendhex(num);
    _uart0_sendstring("\n");
    if (_emmc_status(SR_DAT_INHIBIT | SR_WRITE_AVAILABLE))
    {
        sd_err = SD_TIMEOUT;
        return 0;
    }
    unsigned int* buf = (unsigned int*)buffer;
    if (sd_scr[0] & SCR_SUPP_CCS)
    {
        if (num > 1 && (sd_scr[0] & SCR_SUPP_SET_BLKCNT))
        {
            _emmc_command(CMD_SET_BLOCKCNT,num);
            if (sd_err)
                return 0;
        }
        *EMMC_BLKSIZECNT = (num << 16) | 512;
        _emmc_command(num == 1 ? CMD_WRITE_SINGLE : CMD_WRITE_MULTI, lba);
        if (sd_err)
            return 0;
    }
    else
    {
        *EMMC_BLKSIZECNT = (1 << 16) | 512;
    }
    while (c < num)
    {
        if (!(sd_scr[0] & SCR_SUPP_CCS))
        {
            _emmc_command(CMD_WRITE_SINGLE, (lba + c) * 512);
            if (sd_err)
                return 0;
        }
        if ((r = _emmc_interrupt(INT_WRITE_RDY)))
        {
            _uart0_sendstring("\rERROR: Timeout waiting for ready to write\n");
            sd_err = r;
            return 0;
        }
        for (d = 0; d < 128; d++)
            *EMMC_DATA = buf[d];
        c++;
        buf += 128;
    }
    if ((r = _emmc_interrupt(INT_DATA_DONE)))
    {
        _uart0_sendstring("\rERROR: Timeout waiting for data done\n");
        sd_err = r;
        return 0;
    }
    if (num > 1 && !(sd_scr[0] & SCR_SUPP_SET_BLKCNT) && (sd_scr[0] & SCR_SUPP_CCS))
        _emmc_command(CMD_STOP_TRANS, 0);
    return sd_err != SD_OK || c != num ? 0 : num * 512;
}

// set SD clock to frequency in Hz
int _emmc_clock(unsigned int f)
{
    unsigned int d, c = 41666666 / f;
    unsigned int x, s = 32, h = 0;
    int cnt = 100000;
    while ((*EMMC_STATUS & (SR_CMD_INHIBIT | SR_DAT_INHIBIT)) && cnt--)
        _delay_microseconds(1);
    if (cnt <= 0)
    {
        _uart0_sendstring("ERROR: timeout waiting for inhibit flag\n");
        return SD_ERROR;
    }
    *EMMC_CONTROL1 &= ~C1_CLK_EN;
    _delay_microseconds(10);
    x = c - 1;
    if (!x)
        s = 0;
    else
    {
        if (!(x & 0xffff0000u)) { x <<= 16; s -= 16; }
        if (!(x & 0xff000000u)) { x <<= 8;  s -= 8; }
        if (!(x & 0xf0000000u)) { x <<= 4;  s -= 4; }
        if (!(x & 0xc0000000u)) { x <<= 2;  s -= 2; }
        if (!(x & 0x80000000u)) { x <<= 1;  s -= 1; }
        if (s > 0) s--;
        if (s > 7) s = 7;
    }
    if (sd_hv > HOST_SPEC_V2)
        d = c;
    else
        d = (1 << s);
    if (d <= 2)
    {
        d = 2;
        s = 0;
    }
    _uart0_sendstring("sd_clk divisor ");
    _uart0_sendhex(d);
    _uart0_sendstring(", shift ");
    _uart0_sendhex(s);
    _uart0_sendstring("\n");
    if (sd_hv > HOST_SPEC_V2)
        h = (d & 0x300) >> 2;
    d = (((d & 0x0ff) << 8) | h);
    *EMMC_CONTROL1 = (*EMMC_CONTROL1 & 0xffff003f) | d;
    _delay_microseconds(10);
    *EMMC_CONTROL1 |= C1_CLK_EN;
    _delay_microseconds(10);
    cnt = 10000;
    while (!(*EMMC_CONTROL1 & C1_CLK_STABLE) && cnt--)
        _delay_microseconds(10);
    if (cnt <= 0)
    {
        _uart0_sendstring("ERROR: failed to get stable clock\n");
        return SD_ERROR;
    }
    return SD_OK;
}

int _emmc_init()
{
    long long r, cnt, ccs = 0;
    // GPIO_CD
    r = *GPFSEL4;
    r &= ~(7 << (7 * 3));
    *GPFSEL4 = r;
    *GPPUD = 2;
    _delay_cycles(150);
    *GPPUDCLK1 = (1 << 15);
    _delay_cycles(150);
    *GPPUD = 0;
    *GPPUDCLK1 = 0;
    r = *GPHEN1;
    r |= 1 << 15;
    *GPHEN1 = r;

    // GPIO_CLK, GPIO_CMD
    r = *GPFSEL4;
    r |= (7 << (8 * 3)) | (7 << (9 * 3));
    *GPFSEL4 = r;
    *GPPUD = 2;
    _delay_cycles(150);
    *GPPUDCLK1 = (1 << 16) | (1 << 17);
    _delay_cycles(150);
    *GPPUD = 0;
    *GPPUDCLK1 = 0;

    // GPIO_DAT0, GPIO_DAT1, GPIO_DAT2, GPIO_DAT3
    r = *GPFSEL5;
    r |= (7 << (0 * 3)) | (7 << (1 * 3)) | (7 << (2 * 3)) | (7 << (3 * 3));
    *GPFSEL5 = r;
    *GPPUD = 2;
    _delay_cycles(150);
    *GPPUDCLK1 = (1 << 18) | (1 << 19) | (1 << 20) | (1 << 21);
    _delay_cycles(150);
    *GPPUD = 0;
    *GPPUDCLK1 = 0;

    sd_hv = (*EMMC_SLOTISR_VER & HOST_SPEC_NUM) >> HOST_SPEC_NUM_SHIFT;
    _uart0_sendstring("EMMC: GPIO set up\n");
    // Reset the card.
    *EMMC_CONTROL0 = 0;
    *EMMC_CONTROL1 |= C1_SRST_HC;
    cnt = 10000;
    do
    {
        _delay_microseconds(10);
    } while((*EMMC_CONTROL1 & C1_SRST_HC) && cnt--);
    if (cnt <= 0)
    {
        _uart0_sendstring("ERROR: failed to reset EMMC\n");
        return SD_ERROR;
    }
    _uart0_sendstring("EMMC: reset OK\n");
    *EMMC_CONTROL1 |= C1_CLK_INTLEN | C1_TOUNIT_MAX;
    _delay_microseconds(10);
    // Set clock to setup frequency.
    if ((r = _emmc_clock(400000)))
        return r;
    *EMMC_INT_EN = 0xffffffff;
    *EMMC_INT_MASK = 0xffffffff;
    sd_scr[0] = sd_scr[1] = sd_rca = sd_err = 0;
    _emmc_command(CMD_GO_IDLE, 0);
    if (sd_err)
        return sd_err;

    _emmc_command(CMD_SEND_IF_COND, 0x000001AA);
    if (sd_err)
        return sd_err;
    cnt = 6;
    r = 0;
    while (!(r & ACMD41_CMD_COMPLETE) && cnt--)
    {
        _delay_cycles(400);
        r = _emmc_command(CMD_SEND_OP_COND,ACMD41_ARG_HC);
        _uart0_sendstring("EMMC: CMD_SEND_OP_COND returned ");
        if (r & ACMD41_CMD_COMPLETE)
            _uart0_sendstring("COMPLETE ");
        if (r & ACMD41_VOLTAGE)
            _uart0_sendstring("VOLTAGE ");
        if (r & ACMD41_CMD_CCS)
            _uart0_sendstring("CCS ");
        _uart0_sendhex(r >> 32);
        _uart0_sendhex(r);
        _uart0_sendstring("\n");
        if (sd_err != SD_TIMEOUT && sd_err != SD_OK)
        {
            _uart0_sendstring("ERROR: EMMC ACMD41 returned error\n");
            return sd_err;
        }
    }
    if (!(r & ACMD41_CMD_COMPLETE) || !cnt )
        return SD_TIMEOUT;
    if (!(r & ACMD41_VOLTAGE))
        return SD_ERROR;
    if (r & ACMD41_CMD_CCS)
        ccs = SCR_SUPP_CCS;

    _emmc_command(CMD_ALL_SEND_CID, 0);

    sd_rca = _emmc_command(CMD_SEND_REL_ADDR, 0);
    _uart0_sendstring("EMMC: CMD_SEND_REL_ADDR returned ");
    _uart0_sendhex(sd_rca >> 32);
    _uart0_sendhex(sd_rca);
    _uart0_sendstring("\n");
    if (sd_err)
        return sd_err;

    if ((r = _emmc_clock(25000000)))
        return r;

    _emmc_command(CMD_CARD_SELECT, sd_rca);
    if (sd_err)
        return sd_err;

    if (_emmc_status(SR_DAT_INHIBIT))
        return SD_TIMEOUT;
    *EMMC_BLKSIZECNT = (1 << 16) | 8;
    _emmc_command(CMD_SEND_SCR, 0);
    if (sd_err)
        return sd_err;
    if (_emmc_interrupt(INT_READ_RDY))
        return SD_TIMEOUT;

    r = 0;
    cnt = 100000;
    while (r < 2 && cnt)
    {
        if (*EMMC_STATUS & SR_READ_AVAILABLE)
            sd_scr[r++] = *EMMC_DATA;
        else
            _delay_microseconds(1);
    }
    if (r != 2)
        return SD_TIMEOUT;
    if (sd_scr[0] & SCR_SD_BUS_WIDTH_4)
    {
        _emmc_command(CMD_SET_BUS_WIDTH,sd_rca | 2);
        if (sd_err)
            return sd_err;
        *EMMC_CONTROL0 |= C0_HCTL_DWITDH;
    }
    // add software flag
    _uart0_sendstring("EMMC: supports ");
    if (sd_scr[0] & SCR_SUPP_SET_BLKCNT)
        _uart0_sendstring("SET_BLKCNT ");
    if (ccs)
        _uart0_sendstring("CCS ");
    _uart0_sendstring("\n");
    sd_scr[0] &= ~SCR_SUPP_CCS;
    sd_scr[0] |= ccs;
    return SD_OK;
}
