FROM opensuse/leap:15.5

# Install requirements : update repo and install all requirements
RUN zypper cc -a && \
  zypper install -y gcc gcc-c++ systemd \
    git make python3 bash coreutils \
    rpm-build rpm-devel rpmlint diffutils patch rpmdevtools \
    cmake ninja pkg-config perl golang \
    gtk3-devel gtk4-devel libqt5-qtbase-devel qt6-base-devel \
    zlib-devel c-ares-devel libnghttp2-devel curl-devel \
    http-parser-devel mbedtls-devel && \
  zypper cc -a

# get from content of https://cli.github.com/packages/rpm/gh-cli.repo
# Install requirements : gh
RUN zypper cc -a && \
  zypper mr -da && \
  rpm --import 'https://keyserver.ubuntu.com/pks/lookup?op=get&search=0x23F3D4EA75716059' && \
  zypper addrepo https://cli.github.com/packages/rpm/gh-cli.repo && \
  zypper --gpg-auto-import-keys ref && \
  zypper install -y gh && \
  zypper cc -a

# Install cmake 3.29.8
RUN curl -L https://github.com/Kitware/CMake/releases/download/v3.29.8/cmake-3.29.8-linux-x86_64.tar.gz | \
  tar -C /usr/local --strip-components=1 --gz -xf - && \
  cmake --version
