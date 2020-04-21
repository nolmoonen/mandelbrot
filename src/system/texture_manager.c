#include <stdint.h>
#include <string.h>

#include <util/log.h>
#include <util/counted_list.h>
#include "texture_manager.h"
#include "opengl.h"

int initialize_texture_manager() {
    create_clist(
            &texture_list, TEXTURE_COUNT, sizeof(tex_t),
            (int (*)(void *, uint32_t)) create_tex_from_resource,
            (int (*)(void *, uint32_t)) delete_tex_from_resource
    );

    return EXIT_SUCCESS;
}

int cleanup_texture_manager() {
    delete_clist(&texture_list);

    return EXIT_SUCCESS;
}

int create_tex_from_resource(tex_t *t_texture, uint32_t t_tex_id) {
    // find index of vertex shader
    int32_t tex_index = -1;
    for (uint32_t i = 0; i < sizeof(TEXTURE_RESOURCES) / sizeof(tex_res_t); i++) {
        if (TEXTURE_RESOURCES[i].m_id == t_tex_id) {
            tex_index = i;
            break;
        }
    }
    if (tex_index == -1) {
        nm_log(LOG_ERROR, "\"%d\" is not a registered texture id", t_tex_id);

        return EXIT_FAILURE;
    }

    if (create_tex_from_mem(
            t_texture,
            TEXTURE_RESOURCES[tex_index].m_data, *TEXTURE_RESOURCES[tex_index].m_len, GL_TEXTURE0,
            TEXTURE_RESOURCES[tex_index].m_channel_count
    ) == EXIT_FAILURE) {
        nm_log(LOG_ERROR, "failed to create texture\n");

        return EXIT_FAILURE;
    }

    nm_log(LOG_INFO, "created texture with id \"%d\"\n", t_tex_id);

    return EXIT_SUCCESS;
}

int delete_tex_from_resource(tex_t *t_texture, uint32_t t_tex_id){
    delete_tex(t_texture);

    nm_log(LOG_INFO, "deleted texture with id \"%d\"\n", t_tex_id);

    return EXIT_SUCCESS;
}

int request_texture(tex_t *t_texture, uint32_t t_tex_id) {
    if (get_clist(&texture_list, (void *) t_texture, t_tex_id) == EXIT_FAILURE) {
        nm_log(LOG_ERROR, "could not request texture \"%d\"\n", t_tex_id);

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}