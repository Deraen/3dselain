#include "face.hh"

Face::Face(glm::vec3* a_, glm::vec3* b_, glm::vec3* c_):
    a(a_), b(b_), c(c_),
    normal(glm::normalize(glm::cross((*b - *a), (*c - *a)))),
    d(0.0),
    min(std::min(a->x, std::min(b->x, c->x)),
        std::min(a->y, std::min(b->y, c->y)),
        std::min(a->z, std::min(b->z, c->z))),
    max(std::max(a->x, std::max(b->x, c->x)),
        std::max(a->y, std::max(b->y, c->y)),
        std::max(a->z, std::max(b->z, c->z)))
{
    d = -normal.x * a->x - normal.y * a->y - normal.z * a->z;
}

// http://www.blackpawn.com/texts/pointinpoly/default.html
// Barycentric Technique

// eli selvitetään paljonko A kulmasta liikutaan
// AB ja AC suuntiin. Jos jompaan kumpaan negatiivinen määrä
// piste on AB tai AC viivan väärällä puolella.
// jos yhteensä yli 1 ollaan BC viivan ohi.
bool Face::isPointInside(const glm::vec3 &point) const {
    glm::vec3 v0 = *c - *a;
    glm::vec3 v1 = *b - *a;
    glm::vec3 v2 = point - *a;

    double dot00 = glm::dot(v0, v0);
    double dot01 = glm::dot(v0, v1);
    double dot02 = glm::dot(v0, v2);
    double dot11 = glm::dot(v1, v1);
    double dot12 = glm::dot(v1, v2);

    double inv_denom = 1.0 / (dot00 * dot11 - dot01 * dot01);
    double u = (dot11 * dot02 - dot01 * dot12) * inv_denom;
    double v = (dot00 * dot12 - dot01 * dot02) * inv_denom;

    return (u >= 0) && (v >= 0) && (u + v < 1);
}
