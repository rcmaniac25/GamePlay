#ifndef TILESHEET_H_
#define TILESHEET_H_

#include "SpriteBatch.h"

namespace gameplay
{

class Camera;

/**
 * Defines a collection of images that can be used indepentently of each other or 
 * together to form animations or large scenes.
 */
class TileSheet : public Ref
{
	friend class Sprite;
	friend class SpriteGroup;

public:

	/**
	 * Creates a new TileSheet for drawing individual sprites and tiles from.
	 * 
	 * @param id The ID for the new TileSheet.
	 * @param texture The texture that represents the TileSheet.
     * @param effect Custom effect to be used in the SpriteBatch rendering the TileSheet sprites.
	 * @param initialCapacity An optional initial capacity of the SpriteBatch.
	 * 
	 * @return A new TileSheet for drawing.
	 * @script{create}
	 */
	static TileSheet* create(const char* id, Texture* texture, unsigned int initialCapacity = 0, Effect *effect = NULL);

	/**
     * Creates a TileSheet using the data from the Properties object defined at the specified URL, 
     * where the URL is of the format "<file-path>.<extension>#<namespace-id>/<namespace-id>/.../<namespace-id>"
     * (and "#<namespace-id>/<namespace-id>/.../<namespace-id>" is optional).
	 * 
	 * Note: This cache's the tilesheet internally. Even if the tilesheet is cached, it will increment
	 * the reference count when returned. Example: create(url) with a new url returns a tilesheet with 
	 * ref count of 1. Calling create(url) with the same URL will return a tilesheet with a ref count 
	 * of 2. Be sure to release once you've taken reference so as to ensure that when the last 
	 * reference is complete, it will release and will not cause a memory leak.
     * 
     * @param url The URL pointing to the Properties object defining the TileSheet.
	 * 
	 * @return A new TileSheet for drawing.
     * @script{create}
     */
	static TileSheet* create(const char* url);

	/**
     * Creates a TileSheet from the specified properties object.
     * 
     * @param tileSheetProperties The properties object defining the tilesheet 
	 *  (must have namespace equal to 'tilesheet').
	 * 
	 * @return A new TileSheet for drawing.
     * @script{create}
     */
	static TileSheet* create(Properties* tileSheetProperties);

	/**
     * Get a named TileSheet from its ID.
     *
     * @param id The ID of the TileSheet to search for.
     *
     * @return The TileSheet with the specified ID, or NULL if one was not found.
     */
	static TileSheet* getTileSheet(const char* id);

	/**
     * Gets the identifier for the tile sheet.
     *
     * @return The tile sheet identifier.
     */
    const char* getId() const;

	/**
	 * Gets the SpriteBatch that the tile sheet uses.
	 * 
	 * @return The SpriteBatch from the tile sheet.
	 */
	SpriteBatch* getSpriteBatch() const;

	/**
	 * Gets the number of strips within this tile sheet.
	 * 
	 * @return The number of strips within this tile sheet.
	 */
	unsigned int getStripCount() const;

	/**
	 * Gets the ID of a strip.
	 * 
	 * @param stripIndex The index of a strip within the tile sheet.
	 * 
	 * @return The ID of the strip.
	 */
	const char* getStripId(unsigned int stripIndex) const;

	/**
	 * Gets the index of a strip from ID.
	 * 
	 * @param id The ID of the strip.
	 * 
	 * @return The index of a strip within the tile sheet or -1 if the strip could not be found.
	 */
	int getStripIndex(const char* id) const;

	/**
	 * Gets the number of frames within a strip.
	 * 
	 * @param id The ID of the strip.
	 * 
	 * @return The number of frames within the strip.
	 */
	unsigned int getStripFrameCount(const char* id) const;

	/**
	 * Gets the number of frames within a strip.
	 * 
	 * @param index The index of a strip within the tile sheet.
	 * 
	 * @return The number of frames within the strip.
	 */
	unsigned int getStripFrameCount(unsigned int index) const;

	/**
	 * Add a new strip to the tile sheet.
	 * 
	 * @param id The ID of the new strip.
	 * @param frameCount The number of frames in the new strip. Must be greater then zero.
	 * 
	 * @return The index of the strip.
	 */
	unsigned int addStrip(const char* id, unsigned int frameCount);

	/**
	 * Removes a strip from the tile sheet.
	 * 
	 * @param index The index of a strip to remove.
	 * 
	 * @return A boolean value indicating if the strip was removed or not.
	 */
	bool removeStrip(unsigned int index);

	/**
	 * Removes a strip from the tile sheet.
	 * 
	 * @param id The ID of the strip.
	 * 
	 * @return A boolean value indicating if the strip was removed or not.
	 */
	bool removeStrip(const char* id);

	/**
	 * Gets a frame from a strip.
	 * 
	 * @param stripIndex The index of a strip within the tile sheet.
	 * @param frameIndex The frame to retrieve.
	 * 
	 * @return The frame from the strip, or Rectangle::empty() if it doesn't exist.
	 */
	const Rectangle& getStripFrame(unsigned int stripIndex, unsigned int frameIndex) const;

	/**
	 * Gets a frame from a strip.
	 * 
	 * @param id The ID of the strip.
	 * @param frameIndex The frame to retrieve.
	 * 
	 * @return The frame from the strip, or Rectangle::empty() if it doesn't exist.
	 */
	const Rectangle& getStripFrameById(const char* id, unsigned int frameIndex) const;

	/**
	 * Gets a sequence of frames from a strip.
	 * 
	 * @param stripIndex The index of a strip within the tile sheet.
	 * @param frameIndex The first frame to retrieve.
	 * @param frames The array to store the frames in.
	 * @param frameCount The number of frames to get.
	 */
	void getStripFrames(unsigned int stripIndex, unsigned int frameIndex, Rectangle* frames, unsigned int frameCount) const;

	/**
	 * Gets a sequence of frames from a strip.
	 * 
	 * @param id The ID of the strip.
	 * @param frameIndex The first frame to retrieve.
	 * @param frames The array to store the frames in.
	 * @param frameCount The number of frames to get.
	 */
	void getStripFramesById(const char* id, unsigned int frameIndex, Rectangle* frames, unsigned int frameCount) const;

	/**
	 * Sets a frame within a strip.
	 * 
	 * @param stripIndex The index of a strip within the tile sheet.
	 * @param frameIndex The frame to set.
	 * @param frame The frame to use.
	 */
	void setStripFrame(unsigned int stripIndex, unsigned int frameIndex, const Rectangle& frame);

	/**
	 * Sets a frame within a strip.
	 * 
	 * @param id The ID of the strip.
	 * @param frameIndex The frame to set.
	 * @param frame The frame to use.
	 */
	void setStripFrameById(const char* id, unsigned int frameIndex, const Rectangle& frame);

	/**
	 * Sets an array of frames within a strip.
	 * 
	 * @param stripIndex The index of a strip within the tile sheet.
	 * @param frameIndex The first frame to set.
	 * @param frames The frames to use.
	 * @param frameCount The number of frames to copy.
	 */
	void setStripFrames(unsigned int stripIndex, unsigned int frameIndex, const Rectangle* frames, unsigned int frameCount);

	/**
	 * Sets an array of frames within a strip.
	 * 
	 * @param id The ID of the strip.
	 * @param frameIndex The first frame to set.
	 * @param frames The frames to use.
	 * @param frameCount The number of frames to copy.
	 */
	void setStripFramesById(const char* id, unsigned int frameIndex, const Rectangle* frames, unsigned int frameCount);

	/**
	 * Helper function to start using the internal SpriteBatch.
	 *
	 * @param camera Optional camera where the matrix will be used to adjust the drawn Sprites.
	 * @param viewProjection A boolean value determining if the camera's View-Projection matrix is used, or just the Projection matrix.
	 */
	void startBatch(const Camera* camera = NULL, bool viewProjection = false);

	/**
	 * Helper function to finish using the internal SpriteBatch.
	 */
	void finishBatch();

private:

	class FrameStrip
	{
	public:
		FrameStrip(const char* id, unsigned int frameCount);
		~FrameStrip();

		std::string _id;
		std::vector<Rectangle> _frames;
		unsigned int _frameCount;
	};

    TileSheet(const char* id);

    ~TileSheet();

	static bool loadStrip(TileSheet* sheet, Properties* stripProperties);

    TileSheet(const TileSheet& copy);
    TileSheet& operator=(const TileSheet&);

    std::string _id;
	std::string _path;
	SpriteBatch* _batch;
	std::vector<FrameStrip> _strips;
};

}

#endif
