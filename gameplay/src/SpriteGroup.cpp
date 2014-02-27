#include "Base.h"
#include "SpriteGroup.h"
#include "Node.h"
#include "Scene.h"

namespace gameplay
{

// Global list of active groups
static std::vector<SpriteGroup*> _spriteGroups;

const int SpriteGroup::TYPE_DEFAULT, SpriteGroup::TYPE_CHILD, SpriteGroup::TYPE_TRANSPARENT;

SpriteGroup::SpriteGroup(const char* id)
	: Sprite(id), _groupWidth(1), _groupHeight(1), _children(), _horzGap(0), _vertGap(0), _frustumCull(true)
{
}

SpriteGroup::~SpriteGroup()
{
	//Unreference any Sprites that are used
	for(std::map<unsigned int, Sprite*>::iterator it = _children.begin(); it != _children.end(); it++)
	{
		Sprite* s = it->second;
		if(s)
		{
			SAFE_RELEASE(s);
		}
	}

	// Remove self from vector.
    std::vector<SpriteGroup*>::iterator it = std::find(_spriteGroups.begin(), _spriteGroups.end(), this);
    if (it != _spriteGroups.end())
    {
        _spriteGroups.erase(it);
    }
}

SpriteGroup* SpriteGroup::create(const char* id, unsigned int width, unsigned int height, TileSheet* tileSheet)
{
	GP_ASSERT(width > 0 && height > 0 && tileSheet);
	tileSheet->addRef();

	SpriteGroup* group = new SpriteGroup(id);
	group->_tileSheet = tileSheet;

	group->_groupWidth = width;
	group->_groupHeight = height;

	//Similar to creating from a Sprite, we clone a Sprite to save ourselves the trouble
	Sprite* sp = Sprite::create(NULL, tileSheet);

	NodeCloneContext fake;
	sp->cloneInto(static_cast<Sprite*>(group), fake);

	SAFE_RELEASE(sp);

	//Save the SpriteGroup
	_spriteGroups.push_back(group);

	return group;
}

SpriteGroup* SpriteGroup::create(const char* url)
{
	// Look for the sprite group to see if it was created already
	std::vector<SpriteGroup*>::const_iterator it;
    for (it = _spriteGroups.begin(); it != _spriteGroups.end(); it++)
    {
        SpriteGroup* sg = *it;
        GP_ASSERT(sg);
		if (sg->_path == url)
        {
            return sg;
        }
    }

	// Load the sprite group properties from file.
    Properties* properties = Properties::create(url);
    if (properties == NULL)
    {
        GP_ERROR("Failed to create sprite group from file.");
        return NULL;
    }

    SpriteGroup* group = create((strlen(properties->getNamespace()) > 0) ? properties : properties->getNextNamespace());
    SAFE_DELETE(properties);

	// Remember the path
	if (group)
	{
		group->_path = url;
	}

    return group;
}

static bool getUnsignedIntVector2(Properties* prop, const char* name, Vector2* out)
{
    GP_ASSERT(out);

    const char* valueString = prop->getString(name);
    if (valueString)
    {
        unsigned int x, y;
        int scanned;
        scanned = sscanf(valueString, "%u,%u", &x, &y);
        if (scanned != 2)
        {
            GP_ERROR("Error attempting to parse property '%s' as a two-dimensional unsigned int vector.", name);
            out->set(0.0f, 0.0f);
            return false;
        }

        out->set(x, y);
        return true;
    }
    
    out->set(0.0f, 0.0f);
    return false;
}

static bool getUnsignedIntVector4(Properties* prop, const char* name, Vector4* out)
{
    GP_ASSERT(out);

    const char* valueString = prop->getString(name);
    if (valueString)
    {
        unsigned int x, y, z, w;
        int scanned;
        scanned = sscanf(valueString, "%u,%u,%u,%u", &x, &y, &z, &w);
        if (scanned != 4)
        {
            GP_ERROR("Error attempting to parse property '%s' as a four-dimensional unsigned int vector.", name);
            out->set(0.0f, 0.0f, 0.0f, 0.0f);
            return false;
        }

        out->set(x, y, z, w);
        return true;
    }
    
    out->set(0.0f, 0.0f, 0.0f, 0.0f);
    return false;
}

SpriteGroup* SpriteGroup::create(Properties* spriteGroupProperties)
{
	// Check if the Properties is valid and has a valid namespace.
    if (!spriteGroupProperties || !(strcmp(spriteGroupProperties->getNamespace(), "spritegroup") == 0))
    {
        GP_ERROR("Properties object must be non-null and have namespace equal to 'spritegroup'.");
        return NULL;
    }

	//Get the ID
	const char* id = spriteGroupProperties->getId();
	if (!id || strlen(id) == 0)
    {
        GP_ERROR("SpriteGroup is missing required id.");
		return NULL;
    }

	//Get the size of the group
	Vector2 vec;
	if (!getUnsignedIntVector2(spriteGroupProperties, "groupSize", &vec))
	{
		GP_ERROR("SpriteGroup is missing required groupSize.");
		return NULL;
	}
	unsigned int width = (unsigned int)(vec.x);
	unsigned int height = (unsigned int)(vec.y);

	// Get the tilesheet path.
    const char* tileSheetPath = spriteGroupProperties->getString("tilesheet");
    if (tileSheetPath == NULL || strlen(tileSheetPath) == 0)
    {
        GP_ERROR("SpriteGroup '%s' is missing required tilesheet file path.", id);
		return NULL;
    }

	// Get the tilesheet
	TileSheet* tilesheet = TileSheet::create(tileSheetPath);
	if (!tilesheet)
	{
		GP_ERROR("SpriteGroup '%s' could not load required tilesheet.", id);
		return NULL;
	}

	//Create the group
	SpriteGroup* group = create(id, width, height, tilesheet);
	tilesheet->release();

	//Get gap
	if (spriteGroupProperties->getVector2("gap", &vec))
	{
		group->setGap(vec);
	}

	//Load the normal Sprite properties
	if (!Sprite::loadSpriteProperties(static_cast<Sprite*>(group), spriteGroupProperties, "SpriteGroup"))
	{
		SAFE_RELEASE(group);
		return NULL;
	}

	//Load the map details
	if (!parseGroup(group, spriteGroupProperties))
	{
		GP_ERROR("SpriteGroup '%s' could not load children.", id);
		SAFE_RELEASE(group);
		return NULL;
	}

	return group;
}

bool SpriteGroup::parseGroup(SpriteGroup* group, Properties* spriteGroupProperties)
{
	spriteGroupProperties->rewind();

	//Find the sprites that will be used as children
	Properties* childrenSprites  = NULL;
	bool hasChildren = false;
	Properties* groupProperties = NULL;
	while((groupProperties = spriteGroupProperties->getNextNamespace()))
	{
		if (strcmp(groupProperties->getNamespace(), "sprites") == 0)
		{
			//Found potential children list
			childrenSprites = groupProperties;
		}
		else if(strcmp(groupProperties->getNamespace(), "child") == 0)
		{
			//We have children we need to load
			hasChildren = true;
		}
		else if(strcmp(groupProperties->getNamespace(), "section") == 0)
		{
			const char* type = groupProperties->getString("type");
			if (type && strcmp(type, "child") == 0)
			{
				//Section type is a group of children
				hasChildren = true;
			}
		}
	}

	if (hasChildren && !childrenSprites)
	{
		GP_ERROR("Children exist, but no children exist to be assigned.");
		return false;
	}

	bool result = true;
	Vector2 vec;

	//Parse children
	std::map<std::string, Sprite*> childSprites;
	if (childrenSprites)
	{
		childrenSprites->rewind();
		Properties* childProperties = NULL;
		while ((childProperties = childrenSprites->getNextNamespace()))
		{
			//Load the sprite
			Sprite* s = NULL;
			if (strcmp(childProperties->getNamespace(), "sprite") == 0)
			{
				Sprite* sp;
				if (childProperties->exists("path"))
				{
					const char* url = childProperties->getString("path");
					if (!url || strlen(url) == 0)
					{
						result = false;
						goto parse_error;
					}

					sp = Sprite::create(url);
				}
				else
				{
					sp = Sprite::create(childProperties);
				}

				s = sp;
			}
			else if (strcmp(childProperties->getNamespace(), "spritegroup") == 0)
			{
				SpriteGroup* sg;
				if (childProperties->exists("path"))
				{
					const char* url = childProperties->getString("path");
					if (!url || strlen(url) == 0)
					{
						result = false;
						goto parse_error;
					}

					sg = SpriteGroup::create(url);
				}
				else
				{
					sg = SpriteGroup::create(childProperties);
				}

				s = static_cast<Sprite*>(sg);
			}

			//Save the sprite (so long as the ID isn't in use)
			if (s && childSprites.find(s->getId()) == childSprites.end())
			{
				childSprites[s->getId()] = s;
			}
			else
			{
				if (s)
				{
					SAFE_RELEASE(s);
				}
				result = false;
				goto parse_error;
			}
		}
	}

	//Load group tiles
	spriteGroupProperties->rewind();
	groupProperties = NULL;
	while((groupProperties = spriteGroupProperties->getNextNamespace()))
	{
		const char* type = groupProperties->getNamespace();
		if(strcmp(type, "child") == 0)
		{
			const char* id = groupProperties->getId();
			if (!id || strlen(id) == 0 || !getUnsignedIntVector2(groupProperties, "pos", &vec))
			{
				result = false;
				goto parse_error;
			}

			//Find the sprite
			std::map<std::string, Sprite*>::iterator it = childSprites.find(id);
			if (it == childSprites.end())
			{
				result = false;
				goto parse_error;
			}

			//Set the sprite
			if (!group->setSprite((unsigned int)(vec.x), (unsigned int)(vec.y), it->second))
			{
				result = false;
				goto parse_error;
			}
		}
		else if(strcmp(type, "transparent") == 0)
		{
			if ((!getUnsignedIntVector2(groupProperties, "pos", &vec)) || 
				(!group->setSpriteType((unsigned int)(vec.x), (unsigned int)(vec.y), SpriteGroup::TYPE_TRANSPARENT)))
			{
				result = false;
				goto parse_error;
			}
		}
		else if(strcmp(type, "section") == 0)
		{
			Sprite* s = NULL;

			//Make sure it's a valid type
			type = groupProperties->getString("type");
			if (!(strcmp(type, "transparent") == 0 || strcmp(type, "child") == 0))
			{
				result = false;
				goto parse_error;
			}

			//If it's a child, then get the sprite ID and the sprite
			const char* id = NULL;
			if (strcmp(type, "child") == 0)
			{
				id = groupProperties->getString("id");
				if (!id || strlen(id) == 0)
				{
					result = false;
					goto parse_error;
				}

				//Find the sprite
				std::map<std::string, Sprite*>::iterator it = childSprites.find(id);
				if (it == childSprites.end())
				{
					result = false;
					goto parse_error;
				}
				s = it->second;
			}

			//Get the region itself
			Rectangle region;
			if (!getUnsignedIntVector4(groupProperties, "region", (Vector4*)(&region)))
			{
				result = false;
				goto parse_error;
			}

			//Process the region
			unsigned int x = (unsigned int)region.x;
			unsigned int y = (unsigned int)region.y;
			for (unsigned int i = 0; i < region.width; i++)
			{
				for (unsigned int j = 0; j < region.height; j++)
				{
					if (!group->setSprite(x + i, y + j, s))
					{
						result = false;
						goto parse_error;
					}
				}
			}
		}
	}

parse_error:
	//Cleanup sprites
	std::map<std::string, Sprite*>::iterator it;
	for (it = childSprites.begin(); it != childSprites.end(); it++)
	{
		Sprite* s = it->second;
		SAFE_RELEASE(s);
	}

	return result;
}

SpriteGroup* SpriteGroup::createFrom(const char* id, unsigned int width, unsigned int height, Sprite* sprite)
{
	GP_ASSERT(width > 0 && height > 0 && sprite);

	SpriteGroup* group = SpriteGroup::create(id, width, height, sprite->_tileSheet);

	//Clone the Sprite values
	NodeCloneContext fake;
	sprite->cloneInto(static_cast<Sprite*>(group), fake);

	return group;
}

SpriteGroup* SpriteGroup::getSpriteGroup(const char* id)
{
	GP_ASSERT(id);

    // Search the vector for a matching ID.
    std::vector<SpriteGroup*>::const_iterator it;
    for (it = _spriteGroups.begin(); it != _spriteGroups.end(); ++it)
    {
        SpriteGroup* sg = *it;
        GP_ASSERT(sg);
		if (sg->_id == id)
        {
            return sg;
        }
    }
    return NULL;
}

unsigned int SpriteGroup::getGroupWidth() const
{
	return _groupWidth;
}

unsigned int SpriteGroup::getGroupHeight() const
{
	return _groupHeight;
}

Vector2 SpriteGroup::getGap() const
{
	return Vector2(_horzGap, _vertGap);
}

void SpriteGroup::setGap(const Vector2& gap)
{
	setGap(gap.x, gap.y);
}

void SpriteGroup::setGap(float x, float y)
{
	setHorzGap(x);
	setVertGap(y);
}

float SpriteGroup::getHorzGap() const
{
	return _horzGap;
}

void SpriteGroup::setHorzGap(float gap)
{
	_horzGap = gap;
}

float SpriteGroup::getVertGap() const
{
	return _vertGap;
}

void SpriteGroup::setVertGap(float gap)
{
	_vertGap = gap;
}

int SpriteGroup::getSpriteType(unsigned int x, unsigned int y) const
{
	if(x < _groupWidth && y < _groupHeight)
	{
		unsigned int index = x * _groupWidth + y;
		std::map<unsigned int, Sprite*>::const_iterator it = _children.find(index);
		if(it == _children.end())
		{
			return TYPE_DEFAULT;
		}
		else if(it->second == NULL)
		{
			return TYPE_TRANSPARENT;
		}
		else
		{
			return TYPE_CHILD;
		}
	}
	return -1;
}

bool SpriteGroup::setSpriteType(unsigned int x, unsigned int y, int type)
{
	if(x < _groupWidth && y < _groupHeight && type != TYPE_CHILD && (type == TYPE_DEFAULT || type == TYPE_TRANSPARENT))
	{
		unsigned int index = x * _groupWidth + y;
		std::map<unsigned int, Sprite*>::iterator it = _children.find(index);
		if(it == _children.end())
		{
			if (type == TYPE_TRANSPARENT)
			{
				_children[index] = NULL;
			}
			return true;
		}
		else if(it->second == NULL)
		{
			if (type == TYPE_DEFAULT)
			{
				_children.erase(it);
			}
			return true;
		}
		else
		{
			//Release the child
			Sprite* s = it->second;
			SAFE_RELEASE(s);

			//Set new value
			switch (type)
			{
				case TYPE_DEFAULT:
					_children.erase(it);
					return true;
				case TYPE_TRANSPARENT:
					_children[index] = NULL;
					return true;
			}
		}
	}
	return false;
}

Sprite* SpriteGroup::getSprite(unsigned int x, unsigned int y)
{
	if(x < _groupWidth && y < _groupHeight)
	{
		unsigned int index = x * _groupWidth + y;
		std::map<unsigned int, Sprite*>::const_iterator it = _children.find(index);
		if(it == _children.end())
		{
			return static_cast<Sprite*>(this);
		}
		else
		{
			Sprite* s = it->second;
			if(s != NULL)
			{
				return s;
			}
		}
	}
	return NULL;
}

bool SpriteGroup::setSprite(unsigned int x, unsigned int y, Sprite* sprite)
{
	if(x < _groupWidth && y < _groupHeight && sprite != NULL && sprite != static_cast<Sprite*>(this))
	{
		unsigned int index = x * _groupWidth + y;
		std::map<unsigned int, Sprite*>::const_iterator it = _children.find(index);
		if(it != _children.end())
		{
			//Remove the Sprite if there's one there already
			Sprite* s = it->second;
			if(s)
			{
				SAFE_RELEASE(s);
			}
		}
		//Set the new Sprite
		sprite->addRef();
		_children[index] = sprite;
		return true;
	}
	return setSpriteType(x, y, sprite == NULL ? TYPE_TRANSPARENT : TYPE_DEFAULT);
}

void SpriteGroup::draw(bool isolateDraw)
{
	SpriteBatch* defBatch = _tileSheet->getSpriteBatch();
	std::vector<SpriteBatch*> processedBatches;

	//Determine rotation cemter for entire map
	Vector2 rotationCenter = Vector2(((_width + _horzGap) * _groupWidth) - _horzGap, ((_height + _vertGap) * _groupHeight) - _vertGap) * 0.5f;

	Vector3 pos;
	Vector2 size = Vector2(_width, _height);
	float angle = 0.0f;
	float hGap = _horzGap;
	float vGap = _vertGap;

	const Matrix* vpMatrix = NULL;
	Camera* activeCamera = NULL;

	//Adjust values that rely on Node
	if (Node* node = getNode())
	{
		//Position
		pos = node->getTranslationWorld();

		//Rotation
		const Quaternion& rot = node->getRotation();
		if (rot.x != 0.0f || rot.y != 0.0f || rot.z != 0.0f) //We only want to check these values to determine if we need to calculate rotation.
		{
			angle = atan2f(2.0f * rot.x * rot.y + 2.0f * rot.z * rot.w, 1.0f - 2.0f * ((rot.y * rot.y) + (rot.z * rot.z)));
		}

		//Scale the size
		float sx = node->getScaleX();
		float sy = node->getScaleY();
		size.x *= sx;
		size.y *= sy;
		hGap *= sx;
		vGap *= sy;

		//Get camera
		activeCamera = node->getScene()->getActiveCamera();

		if (isolateDraw && activeCamera)
		{
			//Adjust projection matrix if this draw is specific to this Sprite
			vpMatrix = &(activeCamera->getViewProjectionMatrix());
			defBatch->setProjectionMatrix(*vpMatrix);
		}
	}
	else
	{
		pos = Vector3::zero();
	}

	//Offset position
	pos.x += _x;
	pos.y += _y;

	//Some defaults
	Vector2 defSize(size);
	if ((_flip & FLIP_HORZ) == FLIP_HORZ)
	{
		defSize.x = -defSize.x;
	}
	if ((_flip & FLIP_VERT) == FLIP_VERT)
	{
		defSize.y = -defSize.y;
	}
	Rectangle defSrc = _defaultTileInUse ? _defaultTile : _frame;

	std::map<unsigned int, Sprite*>::const_iterator it = _children.begin();
	bool noChildren = it == _children.end();

	//Prepare for draw loop
	if (isolateDraw)
	{
		defBatch->start();
	}

	//TODO: how can it be made to draw groups within groups? Think of a quad tree, now try drawing it with this

	//Draw loop
	for(unsigned int x = 0; x < _groupWidth; x++)
	{
		Vector3 sTempPos = pos;
		float rX = x * (size.x + hGap);
		sTempPos.x += rX;
		for(unsigned int y = 0; y < _groupHeight; y++)
		{
			sTempPos.y += y ? (size.y + vGap) : 0;

			Vector2 rotationPoint((rotationCenter.x - rX) / size.x, (rotationCenter.y - (y * (size.y + vGap))) / size.y);

			//TODO: If frustum culling is enabled and the rotation angle is not 0, then the bounding box must be adjusted so that it culls correctly (get rid of the "&& angle == 0.0f" to see the issue)

			//"Frustum culling"
			if(activeCamera && _frustumCull && angle == 0.0f)
			{
				if(!activeCamera->getFrustum().intersects(BoundingBox(sTempPos, Vector3(sTempPos.x + size.x, sTempPos.y + size.y, sTempPos.z))))
				{
					//Sprite isn't visible, save the resources and processing

					if(!noChildren)
					{
						//We need to skip specific Sprites
						unsigned int index = x * _groupWidth + y;
						if(it->first == index)
						{
							noChildren = ++it == _children.end();
						}
					}
					continue;
				}
			}

			Vector3 sPos = sTempPos;
			bool drawDefault = true;
			if(!noChildren)
			{
				unsigned int index = x * _groupWidth + y;
				if(it->first == index)
				{
					Sprite* s = it->second;
					noChildren = ++it == _children.end(); //Check if we can skip to just drawing default sprites

					if(s)
					{
						//Custom sprite
						drawDefault = false;

						SpriteBatch* batch = s->_tileSheet->getSpriteBatch();

						if(isolateDraw)
						{
							//Is this a new SpriteBatch?
							std::vector<SpriteBatch*>::iterator bit = std::find(processedBatches.begin(), processedBatches.end(), batch);
							if(bit == processedBatches.end())
							{
								//New SpriteBatch!
								processedBatches.push_back(batch);
								if(vpMatrix)
								{
									batch->setProjectionMatrix(*vpMatrix);
								}
								batch->start();
							}
						}

						//TODO: figure out how to draw multi-sized sprites using this (AKA: don't use size... but then how do we process the values? Keep a list of each one like the SpriteBatches?)

						//Prepare for drawing
						Vector2 sSize = size;

						//Handle flip
						if ((s->_flip & FLIP_HORZ) == FLIP_HORZ)
						{
							sPos.x += sSize.x;
							sSize.x = -sSize.x;
						}
						if ((s->_flip & FLIP_VERT) == FLIP_VERT)
						{
							sPos.y += sSize.y;
							sSize.y = -sSize.y;
						}

						//Draw
						batch->draw(sPos, s->_defaultTileInUse ? s->_defaultTile : s->_frame, sSize, s->_tint, rotationPoint, angle);
					}
					else
					{
						//Transparent, skip
						continue;
					}
				}
			}

			//If nothing was drawn, draw default
			if(drawDefault)
			{
				if ((_flip & FLIP_HORZ) == FLIP_HORZ)
				{
					sPos.x -= defSize.x;
				}
				if ((_flip & FLIP_VERT) == FLIP_VERT)
				{
					sPos.y -= defSize.y;
				}
				defBatch->draw(sPos, defSrc, defSize, _tint, rotationPoint, angle);
			}
		}
	}

	if (isolateDraw)
	{
		defBatch->finish();
		for(std::vector<SpriteBatch*>::iterator bit = processedBatches.begin(); bit != processedBatches.end(); bit++)
		{
			(*bit)->finish();
		}
	}
}

unsigned int SpriteGroup::getAnimationPropertyComponentCount(int propertyId) const
{
	switch (propertyId)
	{
		case ANIMATE_GAP_HORZ:
		case ANIMATE_GAP_VERT:
			return 1;
		case ANIMATE_GAP:
			return 2;
		default:
			return Sprite::getAnimationPropertyComponentCount(propertyId);
	}
}

int SpriteGroup::getPropertyId(TargetType type, const char* propertyIdStr)
{
	GP_ASSERT(propertyIdStr);

	if (type == AnimationTarget::TRANSFORM)
    {
        if (strcmp(propertyIdStr, "ANIMATE_GAP") == 0)
        {
            return SpriteGroup::ANIMATE_GAP;
        }
		else if (strcmp(propertyIdStr, "ANIMATE_GAP_HORZ") == 0)
        {
            return SpriteGroup::ANIMATE_GAP_HORZ;
        }
		else if (strcmp(propertyIdStr, "ANIMATE_GAP_VERT") == 0)
        {
            return SpriteGroup::ANIMATE_GAP_VERT;
        }
	}

	return Sprite::getPropertyId(type, propertyIdStr);
}

void SpriteGroup::getAnimationPropertyValue(int propertyId, AnimationValue* value)
{
	GP_ASSERT(value);

    switch (propertyId)
	{
		case ANIMATE_GAP:
			value->setFloat(0, _horzGap);
			value->setFloat(1, _vertGap);
			break;
		case ANIMATE_GAP_HORZ:
			value->setFloat(0, _horzGap);
			break;
		case ANIMATE_GAP_VERT:
			value->setFloat(0, _vertGap);
			break;
		default:
			Sprite::getAnimationPropertyValue(propertyId, value);
			break;
	}
}

void SpriteGroup::setAnimationPropertyValue(int propertyId, AnimationValue* value, float blendWeight)
{
	GP_ASSERT(value);
    GP_ASSERT(blendWeight >= 0.0f && blendWeight <= 1.0f);

    switch (propertyId)
	{
		case ANIMATE_GAP:
        {
			setGap(Curve::lerp(blendWeight, _horzGap, value->getFloat(0)), Curve::lerp(blendWeight, _vertGap, value->getFloat(1)));
            break;
        }
		case ANIMATE_GAP_HORZ:
        {
			setHorzGap(Curve::lerp(blendWeight, _horzGap, value->getFloat(0)));
            break;
        }
		case ANIMATE_GAP_VERT:
        {
			setVertGap(Curve::lerp(blendWeight, _vertGap, value->getFloat(0)));
            break;
        }
		default:
		{
			Sprite::setAnimationPropertyValue(propertyId, value, blendWeight);
			break;
		}
	}
}

Sprite* SpriteGroup::clone(NodeCloneContext &context)
{
	SpriteGroup* copy = create(getId(), getGroupWidth(), getGroupHeight(), getTileSheet());
	cloneInto(copy, context);
	return static_cast<Sprite*>(copy);
}

void SpriteGroup::cloneInto(SpriteGroup* group, NodeCloneContext &context) const
{
	//Clone Sprite values
	Sprite::cloneInto(static_cast<Sprite*>(group), context);

	//Clone values
	group->_groupWidth = _groupWidth;
	group->_groupHeight = _groupHeight;
	group->_horzGap = _horzGap;
	group->_vertGap = _vertGap;

	//Clone children
	for(std::map<unsigned int, Sprite*>::const_iterator it = _children.begin(); it != _children.end(); it++)
	{
		unsigned int index = it->first;
		if(Sprite* s = it->second)
		{
			//Complex, we don't want to bring over unrelated Nodes...
			if(Node* node = s->getNode())
			{
				//Search through nodes
				Node* clonedNode = context.findClonedNode(node);
				if(Sprite* sc = clonedNode->getSprite())
				{
					//Found it, just reference it and move on
					sc->addRef();
					group->_children[index] = sc;
				}
				else
				{
					//Just clone the node to save us the trouble
					group->_children[index] = s->clone(context);
				}
			}
			else
			{
				//Ok, dodged a bullet. We can just duplicate the Sprite
				group->_children[index] = s->clone(context);
			}
		}
		else
		{
			//Easy, set a NULL
			group->_children[index] = NULL;
		}
	}
}

}

