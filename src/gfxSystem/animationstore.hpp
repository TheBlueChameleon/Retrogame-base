#ifndef ANIMATIONSTORE_HPP
#define ANIMATIONSTORE_HPP

// ========================================================================== //
// Depenencies

// STL
#include <stdexcept>

#include <vector>
#include <string>

#include "animation.hpp"

// ========================================================================== //
// Class

namespace RetrogameBase
{
    class Window;
    class TextureStore;

    class AnimationStore
    {
        private:
            Window& window;
            TextureStore& textureStore;

            std::vector<std::string> filenames;
            std::vector<Animation>   animations;

        public:
            static const size_t NOINDEX = std::numeric_limits<size_t>::max();
            friend class Window;

            // -------------------------------------------------------------- //
            // CTor, DTor

            AnimationStore(Window& window);

            // -------------------------------------------------------------- //
            // getters

            const Window&       getWindow () const;
            const TextureStore& getTextureStore () const;

            size_t size() const;

            Animation& getAnimation(size_t ID);
            const std::string&  getFilename(const int ID) const;
            std::pair<int, int> getAnimationDimension(const int ID) const;

            size_t findByFilename(const std::string& filename) const;         // returns NOINDEX if image not in store

            // -------------------------------------------------------------- //
            // setters/modifiers

            void reset();
        private:
            void reset_private();
        public:

            size_t addAnimation (const std::string& filename);                    // returns index of loaded image in store. Prevents double loading

            void advanceAnimation(size_t ID);
            void advanceAll();
            void resetPhases();

            // -------------------------------------------------------------- //
            // display

            void put (int ID, int x, int y);
            void put (int ID, int x, int y, int angle);
    };
}

#endif // ANIMATIONSTORE_HPP
