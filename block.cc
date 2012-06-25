#include "block.hh"

Block::Block(const std::string& filename, const glm::vec3& origin, const glm::vec3& bounding_box_size):
    Scene(filename),
    center_(origin),
    min_(origin - bounding_box_size * 0.5f),
    max_(origin + bounding_box_size * 0.5f)
{
}

// Kutsutaan loader-threadistä
void Block::visibility(const glm::vec3& pos) {
    // XXX: distance käyttää varmaan sqrt => hidas
    float distance = glm::distance(pos, center_);

    // Molempia voi kutsua vaikka mallia ei ladattu/on ladattu eivätkä tee mitään jossei tarvitse.
    // Hoitavat lukituksen.
    // Jos matkaa kamerasta mallin keskipisteeseen yli x määrä niin pois muistista
    if (distance >= 600.0) unload();
    // Ladataan jos tarpeeksi lähellä
    if (distance <= 500.0) load();
}
