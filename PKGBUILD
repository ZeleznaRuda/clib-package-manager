# Maintainer: Rudolf Muller
pkgname=cclm
pkgver=6.1.2
pkgrel=1
pkgdesc="CCLM C/C++ Library manager - POSIX Only"
arch=('x86_64')
url="https://rudolfmuller.github.io/cclm-library-manager/"
license=('MIT')
depends=()
makedepends=('cmake' 'gcc' 'make')
source=("cclm-$pkgver.tar.gz")
sha256sums=('SKIP')

build() {
  cmake -S . -B build
  cmake --build build
}

package() {
  install -Dm755 build/cclm "$pkgdir/usr/bin/cclm"
  install -Dm644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
