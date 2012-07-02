#include "block.hh"

Block::Block(const std::string& filename, const glm::vec3& origin, const glm::vec3& bounding_box_size, bool street):
    Scene(filename),
    center_(origin),
    min_(origin - bounding_box_size * 0.5f),
    max_(origin + bounding_box_size * 0.5f),
    street_(street)
{
}

// Kutsutaan loader-threadistä
void Block::visibility(const glm::vec3& pos) {
    // XXX: distance käyttää varmaan sqrt => hidas
    float distance = glm::distance(pos, center_);

    // Molempia voi kutsua vaikka mallia ei ladattu/on ladattu eivätkä tee mitään jossei tarvitse.
    // Hoitavat lukituksen.
    // Jos matkaa kamerasta mallin keskipisteeseen yli x määrä niin pois muistista
    if (street_) {
        load();
        return;
    }

    if (distance >= 1100.0) unload();
    // Ladataan jos tarpeeksi lähellä
    if (distance <= 1000.0) load();
}
