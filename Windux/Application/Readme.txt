
When building the Qt application on Linux using the Timesys cross-compiler, you need to
update the PATH environment variable to include the path where it is located.  This will
allow the build to find the correct qmake and cross-compiler (armv7l-timesys-linux-gnueabi-g++).

For example:

	export PATH=/opt/timesys/nitrogen6x/toolchain/ccache:/opt/timesys/nitrogen6x/toolchain/bin:$PATH
