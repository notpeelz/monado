#!/bin/sh
# Copyright 2021, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0
# Author: Moshi Turner <moshiturner@protonmail.com>

if ! type "git-lfs" > /dev/null; then
  echo "Install git-lfs!"
  exit
fi

cd ~/.local/share/monado/hand-tracking-models
git pull
