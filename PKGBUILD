pkgname=glimpse_client
pkgver=0.1
pkgrel=1
pkgdesc="The glimpse measurement client."
arch=('i686' 'x86_64' 'armv7h' 'armv6h')
url="http://www.measure-it.net"
license=('Apache')
depends=('qt5-quickcontrols' 'qt5-base')
makedepends=('libwnck')
source=("${pkgname}-${pkgver}.tar.gz")
sha512sums=('%SHA512SUM%')

build() {
  cd "${srcdir}/${pkgname}-${pkgver}"
  qmake -r
  make
}

package() {
  cd "${srcdir}/${pkgname}-${pkgver}"
  make INSTALL_ROOT="${pkgdir}" install
}
