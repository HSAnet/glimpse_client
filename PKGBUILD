pkgname=glimpse_client

pkgver=0.1

pkgrel=1

pkgdesc="The glimpse measurement client."

arch=('any')

url="http://www.measure-it.net"

license=('Apache')

depends=('qt5-quickcontrols' 'qt5-base')

makedepends=('qt5-base' 'libwnck')

# Source for the package, you can specify a folder (for git or download name to overriding the default
# name saved useful if the upstream use tarballs named 'pinkieþæý' for example
source=("${pkgname}-${pkgver}.tar.gz")

sha512sums=('%SHA512SUM%')

build() {
  cd "${srcdir}/${pkgname}-${pkgver}"
  qmake -r
  make -j4
}

package() {
  cd "${srcdir}/${pkgname}-${pkgver}"
  make INSTALL_ROOT="${pkgdir}" install
}
