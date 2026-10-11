#include "../include/resource.h"

using namespace Engine;

Resource::Resource(const Path& path, const SourceLocation& location)
{
    if (!path.exists())
    {
        throw OrganicParseException("Audio file \"" + path.string() + "\" does not exist.", location);
    }

    if (!path.isFile())
    {
        throw OrganicParseException("\"" + path.string() + "\" is not a file.", location);
    }

    SndfileHandle* file = new SndfileHandle(path.string());

    const sf_count_t length = file->frames() * file->channels();

    const int sampleRate = file->samplerate();
    const int channels = file->channels();

    float* floatSamples = (float*)malloc(sizeof(float) * length);

    if (file->read(floatSamples, length) != length)
    {
        throw OrganicFileException("Could not read audio file \"" + path.string() + "\": " + std::string(file->strError()));
    }

    sf_count_t scaledLength = length;

    if (sampleRate != utils->sampleRate)
    {
        scaledLength = length * (float)utils->sampleRate / sampleRate;

        float* scaledSamples = (float*)malloc(sizeof(float) * scaledLength);

        SRC_DATA data;

        data.data_in = floatSamples;
        data.data_out = scaledSamples;
        data.input_frames = length / channels;
        data.output_frames = scaledLength / channels;
        data.src_ratio = (double)utils->sampleRate / sampleRate;

        const int result = src_simple(&data, SRC_SINC_BEST_QUALITY, channels);

        if (result)
        {
            throw OrganicFileException(std::string("Failed to convert sample rate of audio file \"" + path.string() + "\": ") + src_strerror(result));
        }

        free(floatSamples);

        floatSamples = scaledSamples;
        scaledLength = data.output_frames_gen * utils->channels;
    }

    this->length = scaledLength;

    samples = (double*)malloc(sizeof(double) * scaledLength);

    if (channels == utils->channels)
    {
        for (sf_count_t i = 0; i < scaledLength; i++)
        {
            samples[i] = floatSamples[i];
        }
    }

    else if (channels == 1)
    {
        for (sf_count_t i = 0; i < scaledLength; i++)
        {
            samples[i] = floatSamples[i / 2] / 2;
        }
    }

    else
    {
        for (sf_count_t i = 0; i < scaledLength; i++)
        {
            samples[i / 2] += floatSamples[i];
        }
    }

    free(floatSamples);
}

Resource::Resource() :
    samples(nullptr), length(0) {}

Resource::~Resource()
{
    free(samples);
}

ResourceLocator::ResourceLocator(ValueObject* object) :
    object(object) {}

ResourceLocator::~ResourceLocator()
{
    delete object;

    for (const std::pair<String*, Resource*>& pair : resources)
    {
        delete pair.second;
    }
}

ValueObject* ResourceLocator::getLeaf()
{
    if (!enabled)
    {
        return nullptr;
    }

    return resources.at(object->getLeafAs<String>());
}

void ResourceLocator::loadResources(const SourceLocation& location)
{
    std::unordered_set<ValueObject*> leaves;

    object->getLeaves(leaves);

    for (ValueObject* object : leaves)
    {
        String* path = static_cast<String*>(object);

        if (path->value.empty())
        {
            continue;
        }

        if (!resources.count(path))
        {
            const Path resolved = Path::beside(Path::formatPath(path->value), location.source->path());

            resources[path] = new Resource(resolved, location);
        }
    }
}

void ResourceLocator::init()
{
    object->start(startTime);
}

void ResourceLocator::compute()
{
    object->update();

    if (!object->enabled)
    {
        stop(object->getStopTime());
    }
}
