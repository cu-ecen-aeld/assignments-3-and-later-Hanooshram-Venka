





#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.
#Edits by Hanooshram - added 4 lines for Kernel building process, and in other TO DO sections


set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.15.163
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-



if [ $# -lt 1 ]
then
        OUTDIR=/tmp/aeld #added line here
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$(realpath $1) #added line here 
	echo "Using passed directory ${OUTDIR} for output"
fi

if ! mkdir -p "${OUTDIR}"; then #added lines here to print error message if OUTDIR cant be created
    echo "Error: Directory ${OUTDIR} could not be created."
    exit 1
fi

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}


    # TODO: Add your kernel build steps here, attributes- used the code lines from the module lecture
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} mrproper
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} defconfig
make -j$(nproc) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} all
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} dtbs

fi

echo "Adding the Image in outdir"
cp ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ${OUTDIR}/

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories

mkdir -p "${OUTDIR}/rootfs"
cd "${OUTDIR}/rootfs"

mkdir -p bin dev etc home lib lib64 proc sbin sys tmp usr var
mkdir -p usr/bin usr/lib usr/sbin
mkdir -p var/log

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
else
    cd busybox
fi

 # TODO:  Configure, make and build busybox, Note - I had taken these build and configure commands out of the clone loop, as I was facing issues with busybox building
echo "Configuring and building BusyBox"
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} distclean
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} defconfig
make -j$(nproc) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} CONFIG_PREFIX="${OUTDIR}/rootfs" install


cd "${OUTDIR}/rootfs"

echo "Library dependencies"
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"

# TODO: Add library dependencies to rootfs

SYSROOT=$(${CROSS_COMPILE}gcc -print-sysroot) #this gives the exact path of my crosscompile toolchain's location

cp -a ${SYSROOT}/lib/ld-linux-aarch64.so.1 lib/ #copies the interpreter to /lib
cp -a ${SYSROOT}/lib64/libm.so.6 lib64/ # below shared libraries are copied to /lib
cp -a ${SYSROOT}/lib64/libresolv.so.2 lib64/
cp -a ${SYSROOT}/lib64/libc.so.6 lib64/

# TODO: Make device nodes

sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 600 dev/console c 5 1

# TODO: Clean and build the writer utility
cd "${FINDER_APP_DIR}"
make clean
make CROSS_COMPILE=${CROSS_COMPILE}
cp writer "${OUTDIR}/rootfs/home/"

# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs

cd "${FINDER_APP_DIR}"

cp -a finder.sh finder-test.sh autorun-qemu.sh writer "${OUTDIR}/rootfs/home/"
cp -a -r conf/ "${OUTDIR}/rootfs/home/"
sed -i 's/\.\.\/conf/conf/g' "${OUTDIR}/rootfs/home/finder-test.sh"
sed -i 's|\./writer|/home/writer|g' "${OUTDIR}/rootfs/home/finder-test.sh"
sed -i 's|\./finder.sh|/home/finder.sh|g' "${OUTDIR}/rootfs/home/finder-test.sh"



# TODO: Chown the root directory

cd "${OUTDIR}/rootfs"
sudo chown -R root:root *

# TODO: Create initramfs.cpio.gz
find . | cpio -H newc -ov --owner root:root > "${OUTDIR}/initramfs.cpio"
cd "${OUTDIR}"
gzip -f initramfs.cpio
