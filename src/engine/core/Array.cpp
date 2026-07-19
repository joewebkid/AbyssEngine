

#define GOF2_ARRAY_INSTANTIATIONS
#include "engine/core/Array.h"

#include "engine/math/Vector.h"
#include "engine/math/Matrix.h"
#include "engine/core/AEString.h"
#include "engine/core/KeyFrame.h"
#include "engine/render/ShaderBaseStruct.h"
#include "engine/render/ParticleSettings.h"

namespace AbyssEngine {
    class Mesh;
    class Transform;
    class IApplicationModule;
    class SpriteSystem;
    class AESoundInterface;
    class Camera;
    class Image2D;
    class KeyCode;
    class Material;
    class CheatCode;
    class ImageFont;
    class TokenStruct;
    class AELoadedTexture;
    class Resource;
}

class GameRecord;
class AbstractGun;
class TouchSlider;
class RadioMessage;
class AEGeometry;
class RepairBeam;
class SpacePoint;
class SolarSystem;
class ImagePart;
class TouchButton;
class AELowLevelFile;
class BoundingVolume;
class PendingProduct;
class Gun;
class Item;
class Node;
class Ship;
class Agent;
class Player;
class Wanted;
class Mission;
class Station;
class KIPlayer;
class ListItem;
class NewsItem;
class Waypoint;
class BluePrint;
class Explosion;

namespace ae = AbyssEngine;

template void ArrayRemove<ae::Mesh *>(ae::Mesh *, Array<ae::Mesh *> &);

template void ArrayRemove<ae::String *>(ae::String *, Array<ae::String *> &);

template void ArrayRemove<ae::Transform *>(ae::Transform *, Array<ae::Transform *> &);

template void ArrayRelease<unsigned int>(Array<unsigned int> &);

template void ArrayRelease<ae::AEMath::Matrix>(Array<ae::AEMath::Matrix> &);

template void ArrayRelease<ae::AEMath::Vector>(Array<ae::AEMath::Vector> &);

template void ArrayRelease<ae::IApplicationModule *>(Array<ae::IApplicationModule *> &);

template void ArrayRelease<ae::Mesh *>(Array<ae::Mesh *> &);

template void ArrayAddCached<unsigned int>(unsigned int, Array<unsigned int> &);

template void ArrayAddCached<ae::AEMath::Matrix>(ae::AEMath::Matrix, Array<ae::AEMath::Matrix> &);

template void ArrayAddCached<ae::Mesh *>(ae::Mesh *, Array<ae::Mesh *> &);

template void ArrayRemoveAll<ae::AEMath::Matrix>(Array<ae::AEMath::Matrix> &);

template void ArrayRemoveAll<ae::SpriteSystem *>(Array<ae::SpriteSystem *> &);

template void ArrayRemoveAll<ae::AESoundInterface *>(Array<ae::AESoundInterface *> &);

template void ArrayRemoveAll<ae::Mesh *>(Array<ae::Mesh *> &);

template void ArrayRemoveAll<ae::Camera *>(Array<ae::Camera *> &);

template void ArrayRemoveAll<ae::String *>(Array<ae::String *> &);

template void ArrayRemoveAll<ae::Image2D *>(Array<ae::Image2D *> &);

template void ArrayRemoveAll<ae::KeyCode *>(Array<ae::KeyCode *> &);

template void ArrayRemoveAll<ae::Material *>(Array<ae::Material *> &);

template void ArrayRemoveAll<ae::CheatCode *>(Array<ae::CheatCode *> &);

template void ArrayRemoveAll<ae::ImageFont *>(Array<ae::ImageFont *> &);

template void ArrayRemoveAll<ae::Transform *>(Array<ae::Transform *> &);

template void ArraySetLength<ae::AEMath::Matrix>(unsigned int, Array<ae::AEMath::Matrix> &);

template void ArraySetLength<ae::AEMath::Vector>(unsigned int, Array<ae::AEMath::Vector> &);

template void ArraySetLength<Array<ae::AEMath::Vector *> *>(unsigned int, Array<Array<ae::AEMath::Vector *> *> &);

template void ArraySetLength<Array<ae::String *> *>(unsigned int, Array<Array<ae::String *> *> &);

template void ArraySetLength<ae::AESoundInterface *>(unsigned int, Array<ae::AESoundInterface *> &);

template void ArraySetLength<ae::Mesh *>(unsigned int, Array<ae::Mesh *> &);

template void ArraySetLength<ae::String *>(unsigned int, Array<ae::String *> &);

template void ArrayReleaseClasses<ae::AEMath::Vector *>(Array<ae::AEMath::Vector *> &);

template void ArrayReleaseClasses<Array<ae::AEMath::Vector *> *>(Array<Array<ae::AEMath::Vector *> *> &);

template void ArrayReleaseClasses<Array<ae::String *> *>(Array<Array<ae::String *> *> &);

template void ArrayReleaseClasses<ae::ShaderBaseStruct *>(Array<ae::ShaderBaseStruct *> &);

template void ArrayReleaseClasses<ae::String *>(Array<ae::String *> &);

template void ArrayReleaseClasses<ae::KeyFrame *>(Array<ae::KeyFrame *> &);

template void ArrayAdd<long long>(long long, Array<long long> &);

template void ArrayAdd<ae::AEMath::Matrix>(ae::AEMath::Matrix, Array<ae::AEMath::Matrix> &);

template void ArrayAdd<ParticleSettings::ParticleSet>(ParticleSettings::ParticleSet,
                                                      Array<ParticleSettings::ParticleSet> &);

template void ArrayAdd<ae::TokenStruct *>(ae::TokenStruct *, Array<ae::TokenStruct *> &);

template void ArrayAdd<ae::AELoadedTexture *>(ae::AELoadedTexture *, Array<ae::AELoadedTexture *> &);

template void ArrayAdd<ae::ShaderBaseStruct *>(ae::ShaderBaseStruct *, Array<ae::ShaderBaseStruct *> &);

template void ArrayAdd<ae::IApplicationModule *>(ae::IApplicationModule *, Array<ae::IApplicationModule *> &);

template void ArrayAdd<ae::Camera *>(ae::Camera *, Array<ae::Camera *> &);

template void ArrayAdd<ae::String *>(ae::String *, Array<ae::String *> &);

template void ArrayAdd<ae::Image2D *>(ae::Image2D *, Array<ae::Image2D *> &);

template void ArrayAdd<ae::KeyCode *>(ae::KeyCode *, Array<ae::KeyCode *> &);

template void ArrayAdd<ae::KeyFrame *>(ae::KeyFrame *, Array<ae::KeyFrame *> &);

template void ArrayAdd<ae::Material *>(ae::Material *, Array<ae::Material *> &);

template void ArrayAdd<ae::Resource *>(ae::Resource *, Array<ae::Resource *> &);

template void ArrayAdd<ae::Resource *>(ae::Resource * const *, unsigned int, Array<ae::Resource *> &);

template void ArraySet<ParticleSettings::ParticleSet>(const ParticleSettings::ParticleSet *, unsigned int,
                                                      Array<ParticleSettings::ParticleSet> &);

template void ArraySet<ParticleSettings::ParticleSet>(const Array<ParticleSettings::ParticleSet> &,
                                                      Array<ParticleSettings::ParticleSet> &);

template void ArraySet<ae::KeyFrame *>(ae::KeyFrame * const *, unsigned int, Array<ae::KeyFrame *> &);

template void ArraySet<ae::KeyFrame *>(const Array<ae::KeyFrame *> &, Array<ae::KeyFrame *> &);

template void ArrayAdd<ae::ImageFont *>(ae::ImageFont *, Array<ae::ImageFont *> &);




