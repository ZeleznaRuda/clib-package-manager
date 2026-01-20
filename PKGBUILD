# Maintainer: Rudolf Muller
pkgname=cclm
pkgver=4.4.2
pkgrel=1
pkgdesc="CCLM C/C++ Library manager - POSIX Only"
arch=('x86_64')
url="https://rudolfmuller.github.io/cclm-library-manager/"
license=('MIT')
depends=()
makedepends=('cmake' 'gcc' 'g++' 'make')
source=()
sha256sums=('SKIP')

build() {
  mkdir -p build
  cd build
  cmake ../
  make
}

package() {
  cd build
  install -Dm755 dist/bin/cclm "$pkgdir/usr/bin/cclm"
  install -Dm644 ../LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
