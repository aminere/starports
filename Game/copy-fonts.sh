#!/bin/bash

TARGET=build/iOS8
FONT_DIR=data/common
DEST=${TARGET}/${FONT_DIR}

mkdir -p ${DEST}
cp data/common/default.ttf ${DEST}/
cp data/common/scriptbl.ttf ${DEST}/

