#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <sys/types.h>
#include <malloc.h>
#include <unistd.h>
#include "ak_common.h"
#include "ak_log.h"
#include "ak_mem.h"
#include "ak_thread.h"
#include "ak_uuid.h"

#ifdef AK_RTOS
#include <kernel.h>
#endif

#define LEN_HINT         512
#define LEN_OPTION_SHORT 512

static char *pc_prog_name = NULL;


static char ac_option_hint[  ][ LEN_HINT ] = {                      //操作提示数组
    "help info" ,                                                   //help
    "" ,
};

static struct option option_long[ ] = {
    { "help"      , no_argument       , NULL , 'h' } ,    //"help info"
    {0, 0, 0, 0}
 };

/* example for using this sample */
static void usage(const char * name)
{
    ak_print_normal(MODULE_ID_APP, "Usage: please insmod /usr/modules/ak_efuse.ko first, then %s.\n"
        "cat /sys/kernel/efuse/efuse_OR to compare the value.\n", name);
    ak_print_notice(MODULE_ID_APP, "e.g: %s \n", name);
}


/*
 * help_hint: use the -h --help option.Print option of help information
 * return: 0
 */
static int help_hint(char *pc_prog_name)
{
    int i;

    printf("%s\n" , pc_prog_name);

    /* get opt */
    for(i = 0; i < sizeof(option_long) / sizeof(struct option); i ++) {
        if( option_long[ i ].val != 0 ) {
            printf("\t--%-16s -%c %s\n" , option_long[ i ].name , option_long[ i ].val , ac_option_hint[ i ]);
        }
    }

    /* printf a example */
    usage(pc_prog_name);

    printf("\n\n");
    return 0;
}

/*
 * get_option_short: fill the stort option string.
 * return: option short string addr.
 */
static char *get_option_short( struct option *p_option, int i_num_option, char *pc_option_short, int i_len_option )
{
    int i;
    int i_offset = 0;
    char c_option;

    /* get opt */
    for( i = 0 ; i < i_num_option ; i ++ ) {
        /* opt val */
        if( ( c_option = p_option[ i ].val ) == 0 ) {
            continue;
        }
        switch( p_option[ i ].has_arg ){
        case no_argument:
            /* if no argument, set the offset for default */
            i_offset += snprintf( pc_option_short + i_offset , i_len_option - i_offset , "%c" , c_option );
            break;
        case required_argument:
            /* required argument offset calculate */
            i_offset += snprintf( pc_option_short + i_offset , i_len_option - i_offset , "%c:" , c_option );
            break;
        case optional_argument:
            /* calculate the option offset */
            i_offset += snprintf( pc_option_short + i_offset , i_len_option - i_offset , "%c::" , c_option );
            break;
        }
    }
    return pc_option_short;
}

static int parse_option( int argc, char **argv )
{
    int i_option;
    char ac_option_short[ LEN_OPTION_SHORT ];                               /* opt string */
    int i_array_num = sizeof( option_long ) / sizeof( struct option ) ;     /* get the option num*/
    char c_flag = 1;
    pc_prog_name = argv[ 0 ];                                               /* get the option */

    optind = 0;
    get_option_short( option_long, i_array_num , ac_option_short , LEN_OPTION_SHORT );  /* parse the cmd line input */
    while((i_option = getopt_long(argc , argv , ac_option_short , option_long , NULL)) > 0) {
        switch(i_option) {
        case 'h' :              //help
            help_hint(argv[0]);
            c_flag = 0;
            goto parse_option_end;
        default :
            help_hint(argv[0]); //help message print
            c_flag = AK_FALSE;
            goto parse_option_end;
        }
    }
parse_option_end:
    return c_flag;
}

/**
 * Preconditions:
 * 1. T card is already mounted
 * 2. mic or linein must ready
 */
#ifdef AK_RTOS
static int uuid_sample(int argc, char *argv[])
#else
int main(int argc, char **argv)
#endif
{
    /* start the application */
    sdk_run_config config = {0};
    config.mem_trace_flag = SDK_RUN_NORMAL;
    ak_sdk_init( &config );
    
    ak_print_normal(MODULE_ID_APP, "*****************************************\n");
    ak_print_normal(MODULE_ID_APP, "** uuid demo version: %s **\n", ak_uuid_get_version());
    ak_print_normal(MODULE_ID_APP, "*****************************************\n");

    if (0 == parse_option(argc, argv))
    {
        return 0;
    }
    
    int length = -1;
    int ret =-1;
    
    /* uuid open */
    ret = ak_uuid_open();
    if(ret < 0){
        ak_print_error_ex(MODULE_ID_APP," ak_uuid_open failed\n");
        goto exit;
    }
    unsigned char* global_ptr = NULL;
    ret = ak_uuid_get_global_id(&global_ptr);          // 获取全球id数据
    ret = ak_uuid_get_global_id_length(&length);       // 获取全球id长度
     ak_print_notice_ex(MODULE_ID_APP,"global length= %d\n",length);
    if(ret < 0 ){
        goto exit;
    }
    
    ak_print_notice_ex(MODULE_ID_APP,"global_id=");
    for(int i =0;i< length;i++){
        printf("\033[1;33m %x\033[m", global_ptr[i]);
    }
    printf("\n");

    /* close uuid */
    ak_uuid_close();
exit:
    ak_sdk_exit();
    return ret;
}

#ifdef AK_RTOS
SHELL_CMD_EXPORT_ALIAS(uuid_sample, ak_uuid_sample, UUID Sample);
#endif