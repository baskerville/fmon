#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <limits.h>
#include <sysexits.h>
#include <sys/inotify.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LENGTH (EVENT_SIZE + NAME_MAX + 1)

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

	int i = 0, ret;
	while (i < len) {
		struct inotify_event *event = (struct inotify_event *) &buf[i];
		if (event->mask & IN_OPEN) {
			ret = system(cmd);
			if (ret < 0) {
				err(EX_OSERR, "system");
			}
			break;
		}
		i += EVENT_SIZE + event->len;
	}

	close(fd);
	return ret;
}
