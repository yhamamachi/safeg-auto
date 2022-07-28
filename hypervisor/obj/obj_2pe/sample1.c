#include "hv.h"
#include "syslog.h"

#define TAUD_TO_CNT(us) (((us)*TAUD_PCLK_MHZ)/(1<<TAUD_CLK_PRS))

/*
 *  HVINTを使用するか
 */
#define USE_HVINT

/*
 *  HVINTとして使用するTAUDとOSTMの番号
 */
#define HVINT1_TAUDNO    9
#define HVINT2_OSTMNO    9

/*
 *  コア毎に使用するUARTの番号
 */
const uint uartno[] = {
    RLIN3_PORT0,  /* CORE0 */
    RLIN3_PORT1,  /* CORE1 */
#if TNUM_PHYS_CORE == 4
    RLIN3_PORT2,  /* CORE2 */
    RLIN3_PORT3   /* CORE3 */
#endif /* TNUM_PHYS_CORE == 4 */
};

/*
 *  HV割込みハンドラ0
 */
void
hvint0_handler(void)
{
    ID  coreid;

    GetCoreID(&coreid);

    syslog("HV%d : HVINT0 Handler.\n", coreid);
}

/*
 *  HV割込みハンドラ1
 */
void
hvint1_handler(void)
{
    ID  coreid;

    GetCoreID(&coreid);

    syslog("HV%d : HVINT1 Handler.\n", coreid);
}

/*
 *  タイムウィンドウトリガ割込みハンドラ
 */
void
twdint_handler(void)
{
    ID  coreid;

    GetCoreID(&coreid);

    syslog("\nHV%d : TWTGINT Handler.\n", coreid);
}

/*
 *  HVアイドル処理でのカウント変数
 */
volatile uint32 cnt_idle[TNUM_PHYS_CORE];

/*
 *  HVアイドル処理
 */
void
hv_idle(void)
{
    ID    coreid;
    uint  cnt = 0;

    GetCoreID(&coreid);

    syslog("HV%d : hv_idle : start!\n", coreid);

    while(1) {
        for(cnt_idle[coreid] = 0; cnt_idle[coreid] < 10000000U; cnt_idle[coreid]++);
        syslog("HV%d : hv_idle : running %d.\n", coreid, cnt++);
    }
}

/*
 *  HVタイムウィンドウ処理でのカウント変数
 */
volatile uint32 cnt_twd[TNUM_PHYS_CORE];

/*
 *  HVタイムウィンドウ処理
 */
void
hv_twd(void)
{
    ID      coreid;
    uint    cnt = 0;
    uint32  time_left;
      
    GetCoreID(&coreid);
    syslog("HV%d : hv_twd : start!\n", coreid);

    while(1) {
        for(cnt_twd[coreid] = 0; cnt_twd[coreid] < 10000000U; cnt_twd[coreid]++);
        GetHVTWTimeLeft(&time_left);
        syslog("HV%d : hv_twd : running %d : Time Left %d .\n", coreid, cnt++, time_left);
    }
}

/*
 *  HVIT0として使用するTAUD0の番号
 */
const uint hvint0_taud0no[] = {
    9,  /* CORE0 */
    11, /* CORE1 */
    13, /* CORE2 */
    15  /* CORE3 */
};

/*
 *  ハイパーバイザーの動作モード
 */
#define HV_MODE    1

/*
 *  HVユーザーメイン関数
 */
void
rh850_main(void)
{
    /*
     *  ハイパーバイザーの呼び出し
     */    
    StartHV(HV_MODE);
}

/*
 *  スタートアップフック関数
 */
void
startup_hook(void)
{
    ID    coreid;
    uint  taud0no;

    GetCoreID(&coreid);
    taud0no = hvint0_taud0no[coreid];

#ifdef USE_HVINT
    /*
     *  HVINT0用タイマのスタート
     */
    taud0_start_interval(taud0no, TAUD_TO_CNT((SYSTEM_INTERVAL_US/5)));
#endif /* USE_HVINT */

    /*
     *  UARTの初期化
     */
    uart_init(uartno[coreid], RLIN3xLWBR_VAL, RLIN3xLBRP01_VAL);
    syslog_init(uartno[coreid]);

    /*
     *  各仮想マシンが使用する周辺回路へのアクセス許可設定
     *   VM0_0 : OSTM0 RLIN3_PORT0
     *   VM0_1 : OSTM1 RLIN3_PORT0
     *   VM1_0 : OSTM2 RLIN3_PORT1
     *   VM1_1 : OSTM3 RLIN3_PORT1
     */
    if (coreid == LEADER_COREID) {
        /*
         *  OSTM0へのアクセス権の設定
         */
        sil_wrw_mem(PBGERRSLV50_PBGKCPROT, UNLOCKKEY_VAL);
        sil_wrw_mem(PBG50_PBGPROT1_m(7), (TBIT_HV_SPID|VM0_0_SPIDLIST));
        sil_wrw_mem(PBGERRSLV50_PBGKCPROT, LOCKKEY_VAL);

        /*
         *  OSTM1へのアクセス権の設定
         */
        sil_wrw_mem(PBGERRSLV50_PBGKCPROT, UNLOCKKEY_VAL);
        sil_wrw_mem(PBG50_PBGPROT1_m(8), (TBIT_HV_SPID|VM0_1_SPIDLIST));
        sil_wrw_mem(PBGERRSLV50_PBGKCPROT, LOCKKEY_VAL);

        /*
         *  OSTM2へのアクセス権の設定
         */
        sil_wrw_mem(PBGERRSLV50_PBGKCPROT, UNLOCKKEY_VAL);
        sil_wrw_mem(PBG50_PBGPROT1_m(9), TBIT_HV_SPID|VM1_0_SPIDLIST);
        sil_wrw_mem(PBGERRSLV50_PBGKCPROT, LOCKKEY_VAL);

        /*
         *  OSTM3へのアクセス権の設定
         */
        sil_wrw_mem(PBGERRSLV50_PBGKCPROT, UNLOCKKEY_VAL);
        sil_wrw_mem(PBG50_PBGPROT1_m(10), TBIT_HV_SPID|VM1_1_SPIDLIST);
        sil_wrw_mem(PBGERRSLV50_PBGKCPROT, LOCKKEY_VAL);


#if RLIN3_PORT0 == 0    
        /*
         *  RLIN30へのアクセス権の設定
         */
        sil_wrw_mem(PBGERRSLV30_PBGKCPROT, UNLOCKKEY_VAL);
        sil_wrw_mem(PBG32_PBGPROT1_m(1), (TBIT_HV_SPID|VM0_0_SPIDLIST|VM0_1_SPIDLIST));
        sil_wrw_mem(PBGERRSLV30_PBGKCPROT, LOCKKEY_VAL);
#endif /* RLIN3_PORT0 == 0 */

#if RLIN3_PORT0 == 7
        /*
         *  RLIN37へのアクセス権の設定
         */
        sil_wrw_mem(PBGERRSLV50_PBGKCPROT, UNLOCKKEY_VAL);
        sil_wrw_mem(PBG52_PBGPROT1_m(14), (TBIT_HV_SPID|VM0_0_SPIDLIST|VM0_1_SPIDLIST));
        sil_wrw_mem(PBGERRSLV50_PBGKCPROT, LOCKKEY_VAL);    
#endif /* RLIN3_PORT0 == 7 */

#if RLIN3_PORT1 == 1
        /*
         *  RLIN31へのアクセス権の設定
         */
        sil_wrw_mem(PBGERRSLV50_PBGKCPROT, UNLOCKKEY_VAL);
        sil_wrw_mem(PBG52_PBGPROT1_m(11), (TBIT_HV_SPID|VM1_0_SPIDLIST|VM1_1_SPIDLIST));
        sil_wrw_mem(PBGERRSLV50_PBGKCPROT, LOCKKEY_VAL);
#endif /* RLIN3_PORT1 == 1 */

#if RLIN3_PORT1 == 4
        /*
         *  RLIN34へのアクセス権の設定
         */
        sil_wrw_mem(PBGERRSLV30_PBGKCPROT, UNLOCKKEY_VAL);
        sil_wrw_mem(PBG32_PBGPROT1_m(3), (TBIT_HV_SPID|VM1_0_SPIDLIST|VM1_1_SPIDLIST));
        sil_wrw_mem(PBGERRSLV30_PBGKCPROT, LOCKKEY_VAL);
#endif /* RLIN3_PORT1 == 1 */        
    }

    /*
     *  起動ログの出力
     */
    syslog("HV%d : RH850v2 HyperVisor start on Core%d!\n", coreid, coreid);
}

/*
 *  HVC関数
 */
int
hvc_test0(void)
{
    ID  coreid;

    GetCoreID(&coreid);
//    syslog("HV%d : hvc_test0 : clear cache!\n", coreid);
    set_icctrl(get_icctrl()|ICCTRL_ICHCLR);
    return 1;
}

int
hvc_test1(int arg1)
{
    ID  coreid;

    GetCoreID(&coreid);
//    syslog("HV%d : hvc_test1 : arg1 = %d.\n", coreid, arg1);
    return arg1*2;
}

int
hvc_test2(int arg1, int arg2)
{
    ID  coreid;

    GetCoreID(&coreid);
//    syslog("HV%d : hvc_test2 :  arg1 = %d,  arg2 = %d.\n", coreid, arg1, arg2);
    return (arg1 + arg2)*2;
}

int
hvc_test3(int arg1, int arg2, int arg3)
{
    ID  coreid;

    GetCoreID(&coreid);
//    syslog("HV%d : hvc_test3 :  arg1 = %d,  arg2 = %d,  arg3 = %d.\n", coreid, arg1, arg2, arg3);
    return (arg1 + arg2 + arg3)*2;
}
