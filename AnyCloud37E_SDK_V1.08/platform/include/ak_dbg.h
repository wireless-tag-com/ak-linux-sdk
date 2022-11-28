#ifndef _AK_DBG_H_
#define _AK_DBG_H_

#define DBG_SUPPORT

enum dbg_type {
    DBG_TYPE_PARAM,
    DBG_TYPE_STAT,
};

int ak_debug_info( enum module_id module_id_in, enum dbg_type DBG_TYPE_PARAM, char *pc_data );
const char* ak_dbg_get_version(void);

/**
 * ak_set_debug_str_info - write debug information such as "title=string"
 * @module_id_param[IN]: module id
 * @dbg_type_param[IN]:  debug type 
 * @title[IN]:  debug information title
 * @str[IN]:  debug information string
 * return: 0 success, other failed
 * notes: 
 */
int ak_set_debug_str_info(enum module_id module_id_param, enum dbg_type dbg_type_param, char *title, char *str);

/**
 * ak_set_debug_num_info - write debug information such as "title=number"
 * @module_id_param[IN]: module id
 * @dbg_type_param[IN]:  debug type 
 * @title[IN]:  debug information title
 * @num[IN]:  debug information number
 * return: 0 success, other failed
 * notes: 
 */
int ak_set_debug_num_info(enum module_id module_id_param, enum dbg_type dbg_type_param, char *title, int num);


#endif
