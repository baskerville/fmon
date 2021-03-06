#include <err.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <mntent.h>
#include <sysexits.h>
#include <linux/limits.h>
#include <sys/inotify.h>

#define LENGTH(x)  (sizeof(x) / sizeof(*x))
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LENGTH (EVENT_SIZE + NAME_MAX + 1)
#define ID_PREFIX "file___"
#define TN_SUFFIX_LIBRARY_GRID " - N3_LIBRARY_GRID.parsed"
#define TN_SUFFIX_LIBRARY_FULL " - N3_LIBRARY_FULL.parsed"
#define TN_SUFFIX_FULL " - N3_FULL.parsed"
#define PATH_FORMAT "%s/.kobo-images/%d/%d/%s%s"
#define SD_CARD_PATH "/mnt/onboard"

const char *TN_SUFFIXES[] = {TN_SUFFIX_LIBRARY_GRID, TN_SUFFIX_LIBRARY_FULL, TN_SUFFIX_FULL};

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

/* /mnt/onboard/icons/koreader.png -> file____mnt_onboard_icons_koreader_png */
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

bool is_target_mounted(const char *path)
{
	FILE *mtab = NULL;
	struct mntent *part = NULL;
	bool is_mounted = false;

	if ((mtab = setmntent("/proc/mounts", "r")) != NULL) {
		while ((part = getmntent(mtab)) != NULL) {
			if ((part->mnt_dir != NULL) && (strcmp(part->mnt_dir, path)) == 0) {
				is_mounted = true;
				break;
			}
		}
		endmntent(mtab);
	}

	return is_mounted;
}

void wait_for_target(const char *path)
{
	struct pollfd pfd;
	int mfd = open("/proc/mounts", O_RDONLY, 0);

	pfd.fd = mfd;
	pfd.events = POLLERR | POLLPRI;
	pfd.revents = 0;

	uint8_t tries = 0;

	while (poll(&pfd, 1, -1) >= 0) {
		if (pfd.revents & POLLERR) {
			tries++;
			if (is_target_mounted(path)) {
				break;
			}
		}

		pfd.revents = 0;

		if (tries > 5) {
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		errx(EX_USAGE, "Usage: fmon PATH CMD");
	}

	char *path = argv[1];
	char *cmd = argv[2];

	while (true) {
		int fd = inotify_init();

		if (fd < 0) {
			err(EX_OSERR, "inotify_init");
		}

		int wd = inotify_add_watch(fd, path, IN_OPEN);

		if (wd < 0) {
			err(EX_OSERR, "inotify_add_watch");
		}

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
				int c = 0;
				for (int j = 0; j < LENGTH(TN_SUFFIXES); j++) {
					snprintf(tn, PATH_MAX, PATH_FORMAT, SD_CARD_PATH, dir1, dir2, id, TN_SUFFIXES[j]);
					if (access(tn, F_OK) != 0) {
						break;
					}
					c++;
				}
				free(id);
				if (c == LENGTH(TN_SUFFIXES)) {
					if (system(cmd) < 0) {
						err(EX_OSERR, "system");
					}
				}
			} else {
				if (!is_target_mounted(SD_CARD_PATH)) {
					wait_for_target(SD_CARD_PATH);
				}
			}
			i += EVENT_SIZE + event->len;
		}

		close(fd);
	}

	return EXIT_SUCCESS;
}
