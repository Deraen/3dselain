#include "block.hh"

Block::Block(const std::string& filename, const glm::vec3& origin, const glm::vec3& bounding_box_size, bool street):
    Scene(filename),
    center_(origin),
    min_(origin - bounding_box_size * 0.5f),
    max_(origin + bounding_box_size * 0.5f),
    street_(street)
{
    // Debug::start()[2] << filename << ". Origin:" << origin << ". BBsize:" << bounding_box_size << ". BB: " << min_ << " - " << max_ << Debug::end();
}

// Kutsutaan loader-threadistä
void Block::visibility(const glm::vec3& pos) {

    // jos kappaleen bb (vähän suurennettuna sisällä) niin ladataan

    glm::vec3 bbmin = min_ - glm::vec3(500, 0, 500);
    glm::vec3 bbmax = max_ + glm::vec3(500, 0, 500);

    // Debug::start()[1] << name() << ": " << bbmin << " - " << bbmax << " vs " << pos << Debug::end();

    if (pos.x >= bbmin.x && pos.z >= bbmin.z && pos.x <= bbmax.x && pos.z <= bbmax.z)
        load();

    // Jos ei vähän isomman bb sisällä niin pois muistista

    bbmin -= glm::vec3(200, 0, 200);
    bbmax += glm::vec3(200, 0, 200);

    if (pos.x < bbmin.x || pos.z < bbmin.z || pos.x > bbmax.x || pos.z > bbmax.z)
        unload();

    // XXX: distance käyttää varmaan sqrt => hidas
    // float distance = glm::distance(pos, center_);

    // Molempia voi kutsua vaikka mallia ei ladattu/on ladattu eivätkä tee mitään jossei tarvitse.
    // Hoitavat lukituksen.
    // Jos matkaa kamerasta mallin keskipisteeseen yli x määrä niin pois muistista
    // if (street_) {
        // load();
        // return;
    // }

    // if (distance >= 800.0) unload();
    // Ladataan jos tarpeeksi lähellä
    // if (distance <= 700.0) load();
}
