#!/bin/bash

flatpak-builder --repo=tutorial-repo shadowshuttle-deepin cn.net.pikachu.shadowshuttle-deepin.json --force-clean
flatpak --user uninstall  cn.net.pikachu.shadowshuttle-deepin
flatpak --user install tutorial-repo cn.net.pikachu.shadowshuttle-deepin
flatpak build-bundle tutorial-repo shadowshuttle-deepin.flatpak cn.net.pikachu.shadowshuttle-deepin

