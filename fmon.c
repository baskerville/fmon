#include <err.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <sysexits.h>
#include <linux/limits.h>
#include <sys/inotify.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LENGTH (EVENT_SIZE + NAME_MAX + 1)
#define ID_PREFIX "file___"
#define TN_SUFFIX " - N3_LIBRARY_FULL.parsed"
#define SD_CARD_PATH "/mnt/onboard"

/* QtQHash */
unsigned int qhash(const char *id)
{
	unsigned int h = 0;
	size_t len = strlen(id);

	for (int i = 0; i < len; i++) {
		h = (h << 4) + id[i];
		h ^= (h & 0xf0000000) >> 23;
		h &= 0x0fffffff;
	}

	return h;
}

/* /mnt/onboard/apps/koreader.png -> file____mnt_onboard_apps_koreader_png */
char *id_from_path(const char *path)
{
	size_t len = strlen(path);
	size_t prelen = strlen(ID_PREFIX);
	char *result = malloc(len + prelen + 1);
	strncpy(result, ID_PREFIX, prelen);
	for (int i = 0; i < len; i++) {
		char c = path[i];
		result[i + prelen] = isalnum(c) ? c : '_';
	}
	result[len + prelen] = '\0';
	return result;
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		errx(EX_USAGE, "Usage: fmon PATH CMD");
	}

	char *path = argv[1];
	char *cmd = argv[2];
	int fd = inotify_init();

	if (fd < 0) {
		err(EX_OSERR, "inotify_init");
	}

	int wd = inotify_add_watch(fd, path, IN_OPEN);

	if (wd < 0) {
		err(EX_OSERR, "inotify_add_watch");
	}

	while (true) {
		char buf[BUF_LENGTH];
		int len;

		while (true) {
			len = read(fd, buf, BUF_LENGTH);
			if (len < 0 && errno == EINTR) {
				continue;
			} else {
				break;
			}
		}

		if (len < 0) {
			err(EX_OSERR, "read");
		}

		int i = 0;
		while (i < len) {
			struct inotify_event *event = (struct inotify_event *) &buf[i];
			if (event->mask & IN_OPEN) {
				char *id = id_from_path(path);
				char tn[PATH_MAX];
				unsigned int hash = qhash(id);
				unsigned int dir1 = hash & 0xff;
				unsigned int dir2 = (hash & 0xff00) >> 8;
				snprintf(tn, PATH_MAX, "%s/.kobo-images/%d/%d/%s" TN_SUFFIX,
					 SD_CARD_PATH, dir1, dir2, id);
				free(id);
				if (access(tn, F_OK) == 0) {
					if (system(cmd) < 0) {
						err(EX_OSERR, "system");
					}
				}
			} else {
				goto end;
			}
			i += EVENT_SIZE + event->len;
		}
	}

end:
	close(fd);
	return EXIT_SUCCESS;
}
