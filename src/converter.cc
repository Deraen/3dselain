
#include "scene.hh"
#include "manager.hh"
#include "lumploader.hh"

int main(int argc, char *argv[]) {
    Manager& manager = Manager::instance();

    LumpLoader lumps("assets/obj/000-helsinki-2009_stripped.lpf", LumpLoader::OBJ);
    lumps.load();

    for (unsigned int i = 0; i < manager.objectCount(); ++i) {
        Drawable* obj = manager.getObject(i);

        Scene* scene = dynamic_cast<Scene*>(obj);
        if (scene != NULL) {
            // luetaan meshit tiedostosta
            scene->load();

            // tallennetaan binääri dumppi materiaali / vertex / face datasta
            scene->write();

            scene->unload();
        }
    }

    Manager::instance().unload();
}
