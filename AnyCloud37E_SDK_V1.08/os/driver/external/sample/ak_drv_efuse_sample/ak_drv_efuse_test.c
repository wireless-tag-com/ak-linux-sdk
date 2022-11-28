#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "ak_drv_common.h"

#include "../../include/ak_efuse.h"

#define AK_EFUSE_FILE_PATH "/dev/efuse"

static void display_efuse_global_id(unsigned char *global_id, int len) {
    int tmp = 0;

    printf("Chip globalID: ");
    for (tmp = 0; tmp < len; tmp++) {
        printf("%02X ", global_id[tmp]);
    }
    printf("\n");
}

int ak_drv_read_globalID(void)
{
	int ret;
	int len;
    int fd = -1;
	unsigned char *buffer;

    fd = open(AK_EFUSE_FILE_PATH, O_RDONLY);
    if (fd < 0) {
        ak_print_error("open efuse fail.\n");
        return -EACCES;
    }

    len = ioctl(fd, DO_GLOBE_ID_GET_LEN, NULL);
	if (len > 0) {
		buffer = malloc(len);

		if (buffer) {
			ret = ioctl(fd, DO_GLOBE_ID_GET, buffer);
			if (ret) {
				ak_print_error_ex("read global id fail, ret=%d.\n", ret);
				free(buffer);
				goto fail;
			} else {
				display_efuse_global_id(buffer, len);
			}

			free(buffer);
		} else {
			ak_print_error_ex("malloc %d bytes fail.\n", len);
			ret = -ENOMEM;
			goto fail;
		}
	} else {
        ak_print_error("get global id fail, len=%d.\n", len);
		ret = -EIO;
		goto fail;
	}
    close(fd);

    return 0;
fail:
	return ret;
}
