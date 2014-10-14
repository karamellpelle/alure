
#include "device.h"

#include <string.h>

#include <stdexcept>
#include <algorithm>

#include "alc.h"
#include "alext.h"

#include "context.h"

#if __cplusplus < 201103L
#define final
#endif

namespace alure
{

void ALDevice::removeContext(ALContext *ctx)
{
    std::vector<ALContext*>::iterator iter;
    iter = std::find(mContexts.begin(), mContexts.end(), ctx);
    if(iter != mContexts.end())
        mContexts.erase(iter);
}

std::string ALDevice::getName(PlaybackDeviceType type)
{
    if(type == PlaybackDevType_Complete && !alcIsExtensionPresent(mDevice, "ALC_ENUMERATE_ALL_EXT"))
        type = PlaybackDevType_Basic;
    alcGetError(mDevice);
    const ALCchar *name = alcGetString(mDevice, type);
    if(alcGetError(mDevice) != ALC_NO_ERROR || !name)
        name = alcGetString(mDevice, PlaybackDevType_Basic);
    return std::string(name ? name : "");
}

bool ALDevice::queryExtension(const char *extname)
{
    return alcIsExtensionPresent(mDevice, extname);
}

ALCuint ALDevice::getALCVersion()
{
    ALCint major=-1, minor=-1;
    alcGetIntegerv(mDevice, ALC_MAJOR_VERSION, 1, &major);
    alcGetIntegerv(mDevice, ALC_MINOR_VERSION, 1, &minor);
    if(major < 0 || minor < 0)
        throw std::runtime_error("ALC version error");
    major = std::min<ALCint>(major, std::numeric_limits<ALCushort>::max());
    minor = std::min<ALCint>(minor, std::numeric_limits<ALCushort>::max());
    return MakeVersion((ALCushort)major, (ALCushort)minor);
}

ALCuint ALDevice::getEFXVersion()
{
    if(!alcIsExtensionPresent(mDevice, "ALC_EXT_EFX"))
        return 0;

    ALCint major=-1, minor=-1;
    alcGetIntegerv(mDevice, ALC_EFX_MAJOR_VERSION, 1, &major);
    alcGetIntegerv(mDevice, ALC_EFX_MINOR_VERSION, 1, &minor);
    if(major < 0 || minor < 0)
        throw std::runtime_error("EFX version error");
    major = std::min<ALCint>(major, std::numeric_limits<ALCushort>::max());
    minor = std::min<ALCint>(minor, std::numeric_limits<ALCushort>::max());
    return MakeVersion((ALCushort)major, (ALCushort)minor);
}

ALCuint ALDevice::getFrequency()
{
    ALCint freq = -1;
    alcGetIntegerv(mDevice, ALC_FREQUENCY, 1, &freq);
    if(freq < 0)
        throw std::runtime_error("Frequency error");
    return freq;
}

Context *ALDevice::createContext(ALCint *attribs)
{
    ALCcontext *ctx = alcCreateContext(mDevice, attribs);
    if(!ctx) throw std::runtime_error("Failed to create context");

    ALContext *ret = new ALContext(ctx, this);
    mContexts.push_back(ret);
    return ret;
}

void ALDevice::close()
{
    if(!mContexts.empty())
        throw std::runtime_error("Trying to close device with contexts");

    if(alcCloseDevice(mDevice) == ALC_FALSE)
        throw std::runtime_error("Failed to close device");
    mDevice = 0;

    delete this;
}

}
