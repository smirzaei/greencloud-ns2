#!/bin/sh
# starting from a plain Ubuntu 12.04 32-bit installation, including VM Guest additions

# debian_ok=`cat /etc/debian_version|grep -c wheezy.*sid`
# if [ "$debian_ok" -eq 0 ]
# then
#     echo "I don't think you're using a compatible version of Debian or Ubuntu."
#     echo "Please copy and paste the steps by hand."
#     exit 1
# fi
# 
# 
# echo "
# If you are building a VM, then you should remove unneccessary packages.
# 
# Remove unnecessary packages now? y/[n]:"
# read inputLine



####################### Dependencies (apt) #######################

# if [ "x$inputLine" = "xy" ] || [ "x$inputLine" = "xY" ]
# then
#     sudo apt-get -y remove \
#         brasero totem gnome-orca libreoffice-common libreoffice-core \
#         ubuntu-sounds ttf-indic-fonts-core ttf-punjabi-fonts \
#         samba-common ubuntuone-client launchpad-integration overlay-scrollbar \
#         ubuntu-sounds liboverlay-scrollbar-0.2-0 liboverlay-scrollbar3-0.2-0 unity \
#         gnome-session-canberra ubuntuone-couch trash-cli
# fi
# 
# sudo apt-get update
# sudo apt-get -y install git meld kompare build-essential vim
# sudo apt-get -y upgrade
# sudo apt-get clean


# set some environment variables
mkdir -p build
cd build
BUILD_DIR=`pwd`
LIB_DIR=$BUILD_DIR/../lib
ARCHIVE_DIR=$BUILD_DIR/../upstream
SOURCE_DIR=$BUILD_DIR/../src

#PATH=$PATH:$LIB_DIR:$LIB_DIR/bin
#echo $PATH

# Install X headers
# sudo apt-get -y install xserver-xorg-dev
# XORG_DIR=/usr/include/xorg
# sudo ln -s $XORG_DIR /usr/include/X11

echo "Done with preliminary dependencies. Press enter to start installing source packages, or Ctrl+C to cancel."
#read inputline

####################### Dependencies (from source) #######################

# TCL
if [ "`ls -1 $BUILD_DIR|grep tcl8|wc -l`" -eq 0 ]
then
    cd $BUILD_DIR
    echo $ARCHIVE_DIR/tcl-*gz
    tar -zxvf $ARCHIVE_DIR/tcl-*gz
    ln -s $BUILD_DIR/tcl8* $TCL_DIR
fi
cd $BUILD_DIR/tcl8*
TCL_DIR=`pwd`
cd $TCL_DIR/unix
./configure --prefix=$LIB_DIR
make
##sudo
make install
## need an extra softlink for the other packages
##sudo ln -s /usr/local/bin/tclsh8.5 /usr/local/rin/tclsh
##?ln -s /usr/local/bin/tclsh8.5 /usr/local/rin/tclsh
echo "Done with TCL. Press enter to continue or Ctrl+C to cancel."
#read inputline

#PATH=$PATH:$TCL_DIR


# Tk
if [ "`ls -1 $BUILD_DIR|grep tk|wc -l`" -eq 0 ]
then
    cd $BUILD_DIR
    tar -zxvf $ARCHIVE_DIR/tk-*gz
fi
cd $BUILD_DIR/tk*
TK_DIR=`pwd`
cd $TK_DIR/unix
./configure
make
#sudo make install
ln -s $TK_DIR/unix/libtk8.5.so $LIB_DIR/libtk8.5.so

echo "Done with TK. Press enter to continue or Ctrl+C to cancel."
#read inputline


# OTcl
if [ "`ls -1 $BUILD_DIR|grep otcl|wc -l`" -eq 0 ]
then
    cd $BUILD_DIR
    tar -zxvf $ARCHIVE_DIR/otcl-*gz
fi
cd $BUILD_DIR/otcl*
OTCL_DIR=`pwd`
./configure --with-tcl=$TCL_DIR --with-tcl-ver="8.5.11" --x-includes=$XORG_DIR --prefix=$LIB_DIR
make
#sudo make install

# this is for the run script
#sudo ln -s $OTCL_DIR/libotcl.so /usr/local/lib/libotcl.so
LD_LIBRARY_PATH=$OTCL_DIR:$LIB_DIR:$LIB_DIR/lib/:$LD_LIBRARY_PATH

echo "Done with OTcl. Press enter to continue or Ctrl+C to cancel."
#read inputline



# TclCl
if [ "`ls -1 $BUILD_DIR|grep tclcl|wc -l`" -eq 0 ]
then
    cd $BUILD_DIR
    tar -zxvf $ARCHIVE_DIR/tclcl-*gz
fi
cd $BUILD_DIR/tclcl*
TCLCL_DIR=`pwd`
./configure --x-includes=$XORG_DIR --with-tcl=$TCL_DIR --with-tcl-ver="8.5.11"
make
#sudo make install
echo "Done with TclCl. Press enter to continue or Ctrl+C to cancel."
#read inputline


# NS2
if [ "`ls -1 $BUILD_DIR|grep ns-|wc -l`" -eq 0 ]
then
    cd $BUILD_DIR
    tar -zxvf $ARCHIVE_DIR/ns-*gz
    cd $BUILD_DIR/ns-*
    patch -p0 < ../../upstream/NS2-GreenCloud.diff
fi
cd $BUILD_DIR/ns-*
NS2_DIR=`pwd`
#./configure --with-tclcl=$TCLCL_DIR --with-otcl=$OTCL_DIR --x-libraries=$OTCL_DIR --x-includes=$XORG_DIR --with-tcl=$TCL_DIR --with-tcl-ver="8.5.11"
./configure --prefix=$PWD --with-tclcl=$TCLCL_DIR --with-otcl=$OTCL_DIR --x-libraries=$OTCL_DIR --x-includes=$XORG_DIR  --with-tk=$TK_DIR --with-tclcl=$TCLCL_DIR --with-tcl-ver="8.5.11"
echo "Done configuring NS2. Press enter to continue or Ctrl+C to cancel."
#read inputline


# NAM
#sudo apt-get -y install libxmu-dev
if [ "`ls -1 $BUILD_DIR|grep nam-|wc -l`" -eq 0 ]
then
    cd $BUILD_DIR
    tar -zxvf $ARCHIVE_DIR/nam-*gz
fi
cd $BUILD_DIR/nam-*
NAM_DIR=`pwd`
ZLIB_DIR=`find /usr/lib -name libz.a`
ZLIB_DIR=`dirname "$ZLIB_DIR"`
./configure --with-tclcl=$TCLCL_DIR --with-otcl=$OTCL_DIR --x-libraries=$OTCL_DIR \
    --x-includes=$XORG_DIR --with-tcl=$TCL_DIR --with-zlib=$ZLIB_DIR --with-tcl-ver="8.5.11"
echo "Done configuring NAM. Press enter to continue or Ctrl+C to cancel."
#read inputline


####################### Dashboard #######################
# GreenCloud Dashboard Code
DASHBOARD_DIR=$BUILD_DIR/dashboard
mkdir -p $DASHBOARD_DIR
cd $DASHBOARD_DIR
rsync -av --delete $SOURCE_DIR/dashboard/ ./

# JQuery & UI
unzip $ARCHIVE_DIR/jquery-ui-*
cd $DASHBOARD_DIR/jquery-ui-*
JQUERY_UI_DIR=`pwd`
cd $DASHBOARD_DIR
rm -f jquery
ln -s $JQUERY_UI_DIR jquery

# FLOT JQuery Plotting for Dashboard
unzip $ARCHIVE_DIR/flot-flot-*
cd $DASHBOARD_DIR/flot-flot-*
FLOT_DIR=`pwd`
cd $DASHBOARD_DIR
rm -f flot
ln -s $FLOT_DIR flot

# X/Y Axis Plugin for Flot
# markrcote's version of flot-axislabels: https://github.com/markrcote/flot-axislabels
unzip $ARCHIVE_DIR/flot-axislabels-*.zip
cd $DASHBOARD_DIR/flot-axislabels-*

FLOT_AXIS_DIR=`pwd`
cd $DASHBOARD_DIR
rm -f flot-axislabels
ln -s $FLOT_AXIS_DIR flot-axislabels

# soft-link dashboard.html into the root directory
cd $BUILD_DIR/..
rm -f dashboard.html
# give the softlink a different name so that it can be ignored with .gitignore
ln -s $DASHBOARD_DIR/dashboard.html show-dashboard.html

echo "Done configuring Dashboard, JQuery, and Flot (including flot-axislabels). Press enter to continue or Ctrl+C to cancel."
#read inputline



####################### Global Setup #######################

# Configure enviroment.sh
# $PYTHONPATH and $LD_LIBRARY_PATH have been modified during the install steps above so that this block does something meaningful.

# start with new environment.sh
rm -f $BUILD_DIR/environment.sh.old
mv $BUILD_DIR/environment.sh $BUILD_DIR/environment.sh.old 2>/dev/null
echo '#!/bin/sh' > $BUILD_DIR/environment.sh
echo '# automatically generated by install.sh' >> $BUILD_DIR/environment.sh
echo "" >> $BUILD_DIR/environment.sh

# LD_LIBRARY_PATH
sed -i 's/.*LD_LIBRARY_PATH.*$//g' $BUILD_DIR/environment.sh
echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH" >> $BUILD_DIR/environment.sh
echo "" >> $BUILD_DIR/environment.sh
echo "export PATH=$PATH:$LIB_DIR" >> $BUILD_DIR/environment.sh 


# Final (global) make-install step
cd $BUILD_DIR/.. # back to where we started
make -f Makefile_cluster
#sudo 
make -f Makefile_cluster install
echo "Done with global build. Installation is complete."
echo "Please test it by running the 'run' script in this directory"

