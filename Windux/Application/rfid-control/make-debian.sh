#!/bin/bash

# Get architecture from the command-line argument
ARCH=$1

# Check if architecture is provided
if [ -z "$ARCH" ]; then
    echo "Error: No architecture provided. Usage: ./make-debian.sh <architecture>"
    exit 1
fi

# Set package name, version, and architecture
PACKAGE_NAME="rfid-hook"
VERSION="2.0.1"
PACKAGE_DIR="rfid-control-package"
DEB_PACKAGE="${PACKAGE_NAME}_${VERSION}_${ARCH}.deb"  # Package filename pattern

# Define the target directories for Debian package
DEBIAN_CONTROL_DIR="${PACKAGE_DIR}/DEBIAN"
LIB_DIR="${PACKAGE_DIR}/usr/lib"                       # Target directory for the .so file
SHARE_DIR="${PACKAGE_DIR}/usr/share/rfid-hook"         # Target directory for the entire rfid-hook directory

# Create the directory structure for Debian packaging
mkdir -p "$LIB_DIR" "$SHARE_DIR" "$DEBIAN_CONTROL_DIR"

# Create the DEBIAN control file with the passed architecture
cat <<EOF > "$DEBIAN_CONTROL_DIR/control"
Package: $PACKAGE_NAME
Version: $VERSION
Section: libs
Priority: optional
Architecture: $ARCH
Maintainer: Sean Jordan <sean.jordan@itwfeg.com>
Description: RFID Hook shared library and tools
 This package installs the RFID Hook shared library into /usr/lib and configuration files into /usr/share/rfid-hook.
EOF

# Copy only the shared library to the /usr/lib directory in the package structure
cp output/libHobartCustomerHookRfid.so "$LIB_DIR/"

# Copy only non-.so files and .sim files from the rfid-hook directory to /usr/share/rfid-hook in the Debian package structure
# Avoid copying the .so file into /usr/share, but include .sim files dynamically
find rfid-hook -type f \( ! -name "*.so" -o -name "*.sim" \) -exec cp {} "$SHARE_DIR/" \;

# Set the necessary permissions for the files
chmod -R 755 "$PACKAGE_DIR/usr/"

# Create the control.tar.gz
echo "Creating control.tar.gz..."
tar -czf control.tar.gz -C "$DEBIAN_CONTROL_DIR" .

# Create the data.tar.gz from the package directory (including both /usr/lib and /usr/share)
echo "Creating data.tar.gz..."
tar -czf data.tar.gz -C "$PACKAGE_DIR" usr

# Create the debian-binary file
echo "2.0" > debian-binary

# Remove old .deb files if they exist
rm -f "$DEB_PACKAGE"

# Use `ar` to build the Debian package
echo "Building Debian package: $DEB_PACKAGE"
ar rcs "$DEB_PACKAGE" debian-binary control.tar.gz data.tar.gz

# Cleanup intermediate files
rm -f control.tar.gz data.tar.gz
rm -rf "$PACKAGE_DIR"

# Output message
echo "Package built successfully: ${DEB_PACKAGE}"
