#include "Base.h"
#include "TileSheet.h"
#include "Camera.h"

namespace gameplay
{

// Global list of active sheets
static std::vector<TileSheet*> _tileSheets;

TileSheet::TileSheet(const char* id)
	: _id(id ? id : ""), _batch(NULL), _strips()
{
}

TileSheet::~TileSheet()
{
	SAFE_DELETE(_batch);

	// Remove self from vector.
    std::vector<TileSheet*>::iterator it = std::find(_tileSheets.begin(), _tileSheets.end(), this);
    if (it != _tileSheets.end())
    {
        _tileSheets.erase(it);
    }
}

TileSheet* TileSheet::create(const char* id, Texture* texture, unsigned int initialCapacity, Effect *effect)
{
	SpriteBatch* sb = SpriteBatch::create(texture, effect, initialCapacity);

	TileSheet* tileSheet = new TileSheet(id);
	tileSheet->_batch = sb;

	_tileSheets.push_back(tileSheet);

	return tileSheet;
}

TileSheet* TileSheet::create(const char* url)
{
	// Look for the tilesheet to see if it was created already
	std::vector<TileSheet*>::const_iterator it;
    for (it = _tileSheets.begin(); it != _tileSheets.end(); ++it)
    {
        TileSheet* ts = *it;
        GP_ASSERT(ts);
		if (ts->_path == url)
        {
			ts->addRef();
            return ts;
        }
    }

	// Load the tilesheet properties from file.
    Properties* properties = Properties::create(url);
    if (properties == NULL)
    {
        GP_ERROR("Failed to create tilesheet from file.");
        return NULL;
    }

    TileSheet* tileSheet = create((strlen(properties->getNamespace()) > 0) ? properties : properties->getNextNamespace());
    SAFE_DELETE(properties);

	// Remember the path
	if (tileSheet)
	{
		tileSheet->_path = url;
	}

    return tileSheet;
}

//Taken from Material.cpp (with minor modification)
static Texture::Filter parseTextureFilterMode(const char* str, Texture::Filter defaultValue)
{
    if (str == NULL || strlen(str) == 0)
    {
        return defaultValue;
    }
    else if (strcmp(str, "NEAREST") == 0)
    {
        return Texture::NEAREST;
    }
    else if (strcmp(str, "LINEAR") == 0)
    {
        return Texture::LINEAR;
    }
    else if (strcmp(str, "NEAREST_MIPMAP_NEAREST") == 0)
    {
        return Texture::NEAREST_MIPMAP_NEAREST;
    }
    else if (strcmp(str, "LINEAR_MIPMAP_NEAREST") == 0)
    {
        return Texture::LINEAR_MIPMAP_NEAREST;
    }
    else if (strcmp(str, "NEAREST_MIPMAP_LINEAR") == 0)
    {
        return Texture::NEAREST_MIPMAP_LINEAR;
    }
    else if (strcmp(str, "LINEAR_MIPMAP_LINEAR") == 0)
    {
        return Texture::LINEAR_MIPMAP_LINEAR;
    }
    else
    {
        GP_ERROR("Unsupported texture filter mode string ('%s').", str);
        return defaultValue;
    }
}

static Texture::Wrap parseTextureWrapMode(const char* str, Texture::Wrap defaultValue)
{
    if (str == NULL || strlen(str) == 0)
    {
        return defaultValue;
    }
    else if (strcmp(str, "REPEAT") == 0)
    {
        return Texture::REPEAT;
    }
    else if (strcmp(str, "CLAMP") == 0)
    {
        return Texture::CLAMP;
    }
    else
    {
        GP_ERROR("Unsupported texture wrap mode string ('%s').", str);
        return defaultValue;
    }
}

TileSheet* TileSheet::create(Properties* tileProperties)
{
	// Check if the Properties is valid and has a valid namespace.
    if (!tileProperties || !(strcmp(tileProperties->getNamespace(), "tilesheet") == 0))
    {
        GP_ERROR("Properties object must be non-null and have namespace equal to 'tilesheet'.");
        return NULL;
    }

	//Get the ID
	const char* id = tileProperties->getId();
	if (strlen(id) == 0)
    {
        GP_ERROR("TileSheet is missing required id.");
		return NULL;
    }

	// Get the texture path.
    const char* path = tileProperties->getString("path");
    if (path == NULL || strlen(path) == 0)
    {
        GP_ERROR("TileSheet '%s' is missing required texture file path.", id);
		return NULL;
    }

	//Get texture properties
	bool mipmap = tileProperties->getBool("mipmap");
	Texture::Wrap wrapS = parseTextureWrapMode(tileProperties->getString("wrapS"), Texture::REPEAT);
    Texture::Wrap wrapT = parseTextureWrapMode(tileProperties->getString("wrapT"), Texture::REPEAT);
    Texture::Filter minFilter = parseTextureFilterMode(tileProperties->getString("minFilter"), mipmap ? Texture::NEAREST_MIPMAP_LINEAR : Texture::LINEAR);
    Texture::Filter magFilter = parseTextureFilterMode(tileProperties->getString("magFilter"), Texture::LINEAR);

    const char* vsh = tileProperties->getString("vertexShader");
    const char* fsh = tileProperties->getString("fragmentShader");
    const char* defines = tileProperties->getString("defines");

    // Both vertex and fragment shaders need to be specified (xor check)
    if ((vsh != NULL && strlen(vsh) != 0) ^ (fsh != NULL && strlen(fsh) != 0))
    {
        GP_ERROR("Both fragment and vertex shaders need to be specified if one of them is.");
        return NULL;
    }

    Effect* effect = NULL;
    if (vsh) // vertex and fragment shaders specified
    {
        effect = Effect::createFromFile(vsh, fsh, defines);
    }

	//Get the initial TileSheet
	Texture* texture = Texture::create(path, mipmap);
	TileSheet* tileSheet = TileSheet::create(id, texture, 0, effect);
	SAFE_RELEASE(texture);

	//Modify texture settings
	Texture::Sampler* samp = tileSheet->_batch->getSampler();
	samp->setWrapMode(wrapS, wrapT);
	samp->setFilterMode(minFilter, magFilter);

	//Go through all strips within the sheet
	Properties* stripProperties = NULL;
	while((stripProperties = tileProperties->getNextNamespace()))
	{
		if (strcmp(stripProperties->getNamespace(), "strip") == 0)
        {
            if (!loadStrip(tileSheet, stripProperties))
            {
                GP_ERROR("Failed to load strip for tilesheet.");
                SAFE_RELEASE(tileSheet);
                return NULL;
            }
        }
	}

	return tileSheet;
}

bool TileSheet::loadStrip(TileSheet* sheet, Properties* stripProperties)
{
	GP_ASSERT(sheet);
    GP_ASSERT(stripProperties);

	//Count the number of frames
	unsigned int frameCount = 0;
	stripProperties->rewind();
	Properties* frameProperties = NULL;
	while((frameProperties = stripProperties->getNextNamespace()))
	{
		if (strcmp(frameProperties->getNamespace(), "frame") == 0)
		{
			frameCount++;
		}
	}

	if(frameCount == 0)
	{
		//Need to have 1 or more frames
		return false;
	}

	//Create the strip
	unsigned int index = sheet->addStrip(stripProperties->getId(), frameCount);
	stripProperties->rewind();
	Vector2 defSize;
	frameCount = 0;
	frameProperties = NULL;
	while((frameProperties = stripProperties->getNextNamespace()))
	{
		if (strcmp(frameProperties->getNamespace(), "frame") == 0)
		{
			//Get and validate input
			Vector2 offset;
			if(!frameProperties->getVector2("offset", &offset))
			{
				return false;
			}
			Vector2 size;
			if(!frameProperties->getVector2("size", &size))
			{
				size = defSize;
			}

			if(size.isZero())
			{
				//Can't have a zero-size frame
				return false;
			}

			//If default frame isn't set, set it
			if(defSize.isZero())
			{
				defSize = size;
			}

			//Set the strip frame
			sheet->setStripFrame(index, frameCount++, Rectangle(offset.x, offset.y, size.x, size.y));
		}
	}

	return true;
}

TileSheet* TileSheet::getTileSheet(const char* id)
{
	GP_ASSERT(id);

    // Search the vector for a matching ID.
    std::vector<TileSheet*>::const_iterator it;
    for (it = _tileSheets.begin(); it != _tileSheets.end(); ++it)
    {
        TileSheet* ts = *it;
        GP_ASSERT(ts);
		if (ts->_id == id)
        {
            return ts;
        }
    }
    return NULL;
}

const char* TileSheet::getId() const
{
    return _id.c_str();
}

SpriteBatch* TileSheet::getSpriteBatch() const
{
	return _batch;
}

unsigned int TileSheet::getStripCount() const
{
	return _strips.size();
}

const char* TileSheet::getStripId(unsigned int stripIndex) const
{
	return _strips[stripIndex]._id.c_str();
}

int TileSheet::getStripIndex(const char* id) const
{
	GP_ASSERT(id);

	std::vector<TileSheet::FrameStrip>::const_iterator it;
	int index;
	for (it = _strips.begin(), index = 0; it != _strips.end(); ++it, ++index)
    {
		if ((*it)._id == id)
        {
            return index;
        }
    }
    return -1;
}

unsigned int TileSheet::getStripFrameCount(const char* id) const
{
	return getStripFrameCount((unsigned int)getStripIndex(id));
}

unsigned int TileSheet::getStripFrameCount(unsigned int index) const
{
	GP_ASSERT(index < _strips.size());

	return _strips[index]._frameCount;
}

unsigned int TileSheet::addStrip(const char* id, unsigned int frameCount)
{
	GP_ASSERT(id && frameCount > 0);

	_strips.push_back(TileSheet::FrameStrip(id, frameCount));

	return _strips.size() - 1;
}

bool TileSheet::removeStrip(unsigned int index)
{
	if(index < _strips.size())
	{
		_strips.erase(_strips.begin() + index);
		return true;
	}
	return false;
}

bool TileSheet::removeStrip(const char* id)
{
	return removeStrip((unsigned int)getStripIndex(id));
}

const Rectangle& TileSheet::getStripFrame(unsigned int stripIndex, unsigned int frameIndex) const
{
	GP_ASSERT(stripIndex < _strips.size() && frameIndex < _strips[stripIndex]._frameCount);

	return _strips[stripIndex]._frames[frameIndex];
}

const Rectangle& TileSheet::getStripFrameById(const char* id, unsigned int frameIndex) const
{
	return getStripFrame((unsigned int)getStripIndex(id), frameIndex);
}

void TileSheet::getStripFrames(unsigned int stripIndex, unsigned int frameIndex, Rectangle* frames, unsigned int frameCount) const
{
	GP_ASSERT(stripIndex < _strips.size() && frameIndex < _strips[stripIndex]._frameCount && frames && (frameIndex + frameCount) <= _strips[stripIndex]._frameCount);

	const FrameStrip& strip = _strips[stripIndex];

	std::copy(strip._frames.begin() + frameIndex, strip._frames.begin() + (frameIndex + frameCount), frames);
}

void TileSheet::getStripFramesById(const char* id, unsigned int frameIndex, Rectangle* frames, unsigned int frameCount) const
{
	getStripFrames((unsigned int)getStripIndex(id), frameIndex, frames, frameCount);
}

void TileSheet::setStripFrame(unsigned int stripIndex, unsigned int frameIndex, const Rectangle& frame)
{
	GP_ASSERT(stripIndex < _strips.size() && frameIndex < _strips[stripIndex]._frameCount);

	_strips[stripIndex]._frames[frameIndex] = frame;
}

void TileSheet::setStripFrameById(const char* id, unsigned int frameIndex, const Rectangle& frame)
{
	setStripFrame((unsigned int)getStripIndex(id), frameIndex, frame);
}

void TileSheet::setStripFrames(unsigned int stripIndex, unsigned int frameIndex, const Rectangle* frames, unsigned int frameCount)
{
	GP_ASSERT(stripIndex < _strips.size() && frameIndex < _strips[stripIndex]._frameCount && frames && (frameIndex + frameCount) <= _strips[stripIndex]._frameCount);

	FrameStrip& strip = _strips[stripIndex];

	std::copy(frames, frames + frameCount, strip._frames.begin() + frameIndex);
}

void TileSheet::setStripFramesById(const char* id, unsigned int frameIndex, const Rectangle* frames, unsigned int frameCount)
{
	setStripFrames((unsigned int)getStripIndex(id), frameIndex, frames, frameCount);
}

void TileSheet::startBatch(const Camera* camera, bool viewProjection)
{
	_batch->start();

	if(camera)
	{
		_batch->setProjectionMatrix(viewProjection ? camera->getViewProjectionMatrix() : camera->getProjectionMatrix());
	}
}

void TileSheet::finishBatch()
{
	_batch->finish();
}

//FrameStrip
TileSheet::FrameStrip::FrameStrip(const char* id, unsigned int frameCount)
	: _id(id ? id : ""), _frames(frameCount), _frameCount(frameCount)
{
}

TileSheet::FrameStrip::~FrameStrip()
{
}

}
