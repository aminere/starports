#!/bin/bash

PLATFORM=$1

CKTOOL_PLATFORM=$PLATFORM

PROJECT_DIR=../../../../../../Game
RAW_AUDIO_DIR=$PROJECT_DIR/rawdata/audio

if [ $PLATFORM == macosx ]; then
	AUDIO_DIR=$PROJECT_DIR/build/iOS8/audio
elif [ $PLATFORM == wp8 ]; then
	AUDIO_DIR=$PROJECT_DIR/build/vc11phone/Assets
else
	AUDIO_DIR=$PROJECT_DIR/audio
fi

echo AUDIO_DIR: $AUDIO_DIR

cd ../shoot/utils/cricket/bin/$CKTOOL_PLATFORM/

#if [ $PLATFORM == win8 ]; then
	cd x86
#fi

if [ $PLATFORM != wp8 ]; then
	mkdir -p $AUDIO_DIR
fi

# Compile banks
BANKS=`find $RAW_AUDIO_DIR/*.ckbx`
for BANK in $BANKS; do
	BANK_NAME=`basename $BANK`
	BANK_NAME_NO_EXT=`echo $BANK_NAME | cut -d"." -f1`
	if [ $PLATFORM == wp8 ]; then
		./cktool buildbank $BANK $AUDIO_DIR/audio-$BANK_NAME_NO_EXT.ckb
	else
		./cktool buildbank $BANK $AUDIO_DIR/$BANK_NAME_NO_EXT.ckb
	fi
done

# Copy OGGs
OGGS=`find $RAW_AUDIO_DIR/*.ogg`
for OGG in $OGGS; do
	OGG_NAME=`basename $OGG`
	
	if [ $PLATFORM == wp8 ]; then
		DEST_OGG=$AUDIO_DIR/audio-$OGG_NAME
	else
		DEST_OGG=$AUDIO_DIR/$OGG_NAME
	fi
	
	cp $OGG $DEST_OGG
	chmod 777 $DEST_OGG
done

read -n 1 -p "Press any key to continue"
