#ifndef BOMBER_TEXTURE_MANAGER_H
#define BOMBER_TEXTURE_MANAGER_H

#include <stdlib.h>

#include <util/counted_list.h>
#include "opengl.h"

#define TEX_GRASS 0
#define TEX_CRATE 1
#define TEX_CHARACTER 2

/**
 * Specified manually based on identifiers in CMakeLists.
 */
extern const char grass[];
extern const size_t grass_len;

extern const char crate[];
extern const size_t crate_len;

extern const char character[];
extern const size_t character_len;

typedef struct {
    /** manually specified identifier, used to request the texture */
    uint32_t m_id;
    /** pointers to extern embedded data */
    const char *m_data;
    const size_t *m_len;
    /** properties of the texture, need to be manually defined */
    uint32_t m_channel_count;
} tex_res_t;

/** Array to obtain the desired data using an id. */
static const tex_res_t TEXTURE_RESOURCES[] = {
        {.m_id=TEX_GRASS, .m_data=grass, .m_len=&grass_len, .m_channel_count=4},
        {.m_id=TEX_CRATE, .m_data=crate, .m_len=&crate_len, .m_channel_count=3},
        {.m_id=TEX_CHARACTER, .m_data=character, .m_len=&character_len, .m_channel_count=3}
};

static const size_t TEXTURE_COUNT = 10;

clist_t texture_list;

int initialize_texture_manager();

int cleanup_texture_manager();

/** @private Creates a texture in {@param t_texture} based on {@param t_tex_id} from TEXTURE_RESOURCES. */
int create_tex_from_resource(tex_t *t_texture, uint32_t t_tex_id);

/** @private Deletes a texture in {@param t_texture} based on {@param t_tex_id} from TEXTURE_RESOURCES. */
int delete_tex_from_resource(tex_t *t_texture, uint32_t t_tex_id);

/** Returns a texture in {@param t_texture} based on {@param t_tex_id} from TEXTURE_RESOURCES. */
int request_texture(tex_t *t_texture, uint32_t t_tex_id);

#endif //BOMBER_TEXTURE_MANAGER_H
