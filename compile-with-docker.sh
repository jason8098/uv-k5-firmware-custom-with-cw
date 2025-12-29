#!/bin/sh
#export DOCKER_DEFAULT_PLATFORM=linux/amd64
#export DOCKER_NETWORK="--network=host"
IMAGE_NAME="uvk5"
rm "${PWD}/compiled-firmware/*"
echo "Building docker image $IMAGE_NAME"
if ! docker build -t $DOCKER_NETWORK $IMAGE_NAME .
then
    echo "Failed to build docker image"
    exit 1
fi

custom() {
    echo "Custom compilation..."
    docker run --rm -v "${PWD}/compiled-firmware/:/app/compiled-firmware" $IMAGE_NAME /bin/bash -c "rm ./compiled-firmware/*; cd /app && make -s \
        EDITION_STRING=Custom \
        TARGET=k5cw-beacon.custom \
        && cp k5cw-beacon.custom* compiled-firmware/"
}

standard() {
    echo "Standard compilation..."
    docker run --rm -v "${PWD}/compiled-firmware:/app/compiled-firmware" $IMAGE_NAME /bin/bash -c "rm ./compiled-firmware/*; cd /app && make -s \
        ENABLE_SPECTRUM=0 \
        ENABLE_FMRADIO=0 \
        ENABLE_AIRCOPY=0 \
        ENABLE_NOAA=0 \
        EDITION_STRING=Standard \
        TARGET=f4hwn.standard \
        && cp f4hwn.standard* compiled-firmware/"
}

bandscope() {
    echo "Bandscope compilation..."
    docker run --rm -v "${PWD}/compiled-firmware/:/app/compiled-firmware" $IMAGE_NAME /bin/bash -c "rm ./compiled-firmware/*; cd /app && make -s \
        ENABLE_SPECTRUM=1 \
        ENABLE_FMRADIO=0 \
        ENABLE_AIRCOPY=1 \
        ENABLE_FEAT_F4HWN_PMR=1 \
        ENABLE_FEAT_F4HWN_GMRS_FRS_MURS=1 \
        ENABLE_NOAA=0 \
        ENABLE_FEAT_F4HWN_RESCUE_OPS=0 \
        EDITION_STRING=Bandscope \
        TARGET=f4hwn.bandscope \
        && cp f4hwn.bandscope* compiled-firmware/"
}

broadcast() {
    echo "Broadcast compilation..."
    docker run --rm -v "${PWD}/compiled-firmware:/app/compiled-firmware" $IMAGE_NAME /bin/bash -c "cd /app && make -s \
        ENABLE_SPECTRUM=0 \
        ENABLE_FMRADIO=1 \
        ENABLE_AIRCOPY=1 \
        ENABLE_FEAT_F4HWN_PMR=1 \
        ENABLE_FEAT_F4HWN_GMRS_FRS_MURS=1 \
        ENABLE_NOAA=0 \
        EDITION_STRING=Broadcast \
        ENABLE_FEAT_F4HWN_RESCUE_OPS=0 \
        TARGET=f4hwn.broadcast \
        && cp f4hwn.broadcast* compiled-firmware/"
}

basic() {
    echo "Basic compilation..."
    docker run --rm -v "${PWD}/compiled-firmware:/app/compiled-firmware" $IMAGE_NAME /bin/bash -c "cd /app && make -s \
        ENABLE_SPECTRUM=1 \
        ENABLE_FMRADIO=1 \
        ENABLE_VOX=0 \
        ENABLE_AIRCOPY=0 \
        ENABLE_AUDIO_BAR=0 \
        ENABLE_FEAT_F4HWN_SPECTRUM=0 \
        ENABLE_FEAT_F4HWN_PMR=1 \
        ENABLE_FEAT_F4HWN_GMRS_FRS_MURS=1 \
        ENABLE_NOAA=0 \
        ENABLE_FEAT_F4HWN_RESUME_STATE=0 \
        ENABLE_FEAT_F4HWN_CHARGING_C=0 \
        ENABLE_FEAT_F4HWN_INV=1 \
        ENABLE_FEAT_F4HWN_CTR=0 \
        ENABLE_FEAT_F4HWN_NARROWER=0 \
        ENABLE_FEAT_F4HWN_RESCUE_OPS=0 \
        EDITION_STRING=Basic \
        TARGET=f4hwn.basic \
        && cp f4hwn.basic* compiled-firmware/"
}

rescueops() {
    echo "RescueOps compilation..."
    docker run --rm -v "${PWD}/compiled-firmware:/app/compiled-firmware" $IMAGE_NAME /bin/bash -c "cd /app && make -s \
        ENABLE_SPECTRUM=0 \
        ENABLE_FMRADIO=0 \
        ENABLE_AIRCOPY=1 \
        ENABLE_FEAT_F4HWN_PMR=1 \
        ENABLE_FEAT_F4HWN_GMRS_FRS_MURS=1 \
        ENABLE_NOAA=1 \
        ENABLE_FEAT_F4HWN_RESCUE_OPS=1 \
        EDITION_STRING=RescueOps \
        TARGET=f4hwn.rescueops \
        && cp f4hwn.rescueops* compiled-firmware/"
}

case "$1" in
    custom)
        custom
        ;;
    standard)
        standard
        ;;
    bandscope)
        bandscope
        ;;
    broadcast)
        broadcast
        ;;
    basic)
        basic
        ;;
    rescueops)
        rescueops
        ;;
    all)
        bandscope
        broadcast
        rescueops
        basic
        ;;
    *)
        echo "Usage: $0 {custom|bandscope|broadcast|basic|standard|all}"
        exit 1
        ;;
esac