## 概要

mtdrw は Linux 向けのローダブル・カーネルモジュールで、本モジュールをロードしている間、論理的にリードオンリーとなっている MTD デバイスをリードライト状態とすることができます。

本モジュールは Linux v2.6.26-at*X* が動作する Atmark Techno 社の Armadillo-4x0 シリーズの CPU ボード専用です。

* http://armadillo.atmark-techno.com/armadillo-420
* http://armadillo.atmark-techno.com/armadillo-440

Armadillo-4x0 でより新しい Linux v3.14-at*X* を使用している場合は、下記のように本モジュールと同等のことを行うための sysfs インタフェースが既に提供されているため、本モジュールを使用する必要はありません。

``` r
# echo 0 > /sys/class/mtd/mtd0/ro
```

本モジュールを使用することで、Armadillo-4x0 のブートローダーをユーザーランドから netflash コマンドを使用して更新することができます。Armadillo-4x0 のブートローダーは NOR フラッシュの第一パーティションに書き込まれていますが、NOR フラッシュの第一パーティションはリードオンリーとしてマークされているため通常はユーザーランドから書き込むことはできません。

## ビルド

mtdrw のビルドは ATDE5 上で実施する必要があります。ATDE5 は VMware 仮想マシン上で動作する Armadillo-4x0 向けのクロスコンパイル環境です。ビルドを行う前に、予め ATDE5 をセットアップしてください。詳細は <http://armadillo.atmark-techno.com/atde> を参照ください。

最初に Armadillo-4x0 のカーネルをビルドします。
``` r
$ cd
$ wget http://armadillo.atmark-techno.com/files/downloads/armadillo-420/source/kernel/linux-2.6.26-at27.tar.gz
$ tar -zxvf linux-2.6.26-at27.tar.gz
$ cd linux-2.6.26-at27
$ make ARCH=arm armadillo400_defconfig
$ make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
```

次に mtdrw をビルドします。
``` r
$ cd
$ git clone https://github.com/g3y7spk/mtdrw.git
$ cd mtdrw
$ make
```
ビルド後、カレントディレクトリ内にローダブル・カーネルモジュール (mtdrw.ko) が生成されます。

NOTE: 上記とは別バージョンのカーネルを使用している場合は、mtdrw/Makefile 内の KERNEL_SRC を必要に応じて変更してください。

``` r
KERNEL_SRC = /home/atmark/linux-2.6.26-at27

ARCH = arm
CROSS_COMPILE = arm-linux-gnueabi-

obj-m += mtdrw.o
clean-files += modules.order

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_SRC) M=$(PWD) modules

clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_SRC) M=$(PWD) clean
```

## 使用方法
mtdrw は MTD デバイスの名称を指定するための *name* パラメータを１つ指定する必要があります。Armadillo-4x0 では下記の名称が MTD デバイスに設定されています。

* nor.bootloader
* nor.kernel
* nor.userland
* nor.config

下記に例として Armadillo-4x0 のブートローダーを更新する際のコマンド実行手順を記載します。コマンドは Armadillo-4x0 上で実行する必要があります。尚、mtdrw.ko と loader-armadillo4x0-v2.2.0.bin が Armadillo-4x0 の /tmp ディレクトリにあると想定します。mtdrw.ko はビルドしたローダブル・カーネルモジュール、そして loader-armadillo4x0-v2.0.0.bin は Armadillo-4x0 向けのブートローダーイメージで、<http://armadillo.atmark-techno.com/armadillo-420/downloads-linux2-6-26> から入手可能です。

``` r
# cd /tmp
# insmod ./mtdrw.ko name=nor.bootloader
Using ./mtdrw.ko
mtd0 is writeable now
# netflash -b -k -n -u -s -r /dev/flash/bootloader ./loader-armadillo4x0-v2.2.0.bin
# rmmod mtdrw
mtd0 is read-only again
```
