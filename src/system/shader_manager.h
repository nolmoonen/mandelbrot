#ifndef NM_SHADER_MANAGER_H
#define NM_SHADER_MANAGER_H

#include <util/counted_list.h>
#include "opengl.h"

#define SHADER_DEFAULT 0

// todo optionally also buffer shaders instead of only shader programs
/**
 * Specified manually based on identifiers in CMakeLists.
 */
extern const char default_vert[];
extern const size_t default_vert_len;

extern const char default_frag[];
extern const size_t default_frag_len;

typedef struct {
    uint32_t m_id;
    /** pointers to extern embedded data */
    const char *m_vert_text;
    const size_t *m_vert_len;
    const char *m_frag_text;
    const size_t *m_frag_len;
} shader_res_t;

/**
 * Array to obtain the desired data using an id.
 */
static const shader_res_t SHADER_PROGRAM_RESOURCES[] = {
        {
                .m_id=SHADER_DEFAULT,
                .m_vert_text=default_vert, .m_vert_len=&default_vert_len,
                .m_frag_text=default_frag, .m_frag_len=&default_frag_len
        }
};

static const size_t SHADER_COUNT = 10;

clist_t shader_list;

int initialize_shader_manager();

int cleanup_shader_manager();

/** Returns a shader program in {@param t_program} based on {@param t_prog_id} from SHADER_PROGRAM_RESOURCES. */
int request_program(shader_program_t *t_program, uint32_t t_prog_id);

/** @private Creates a shader program in {@param t_program} based on {@param t_prog_id} from SHADER_PROGRAM_RESOURCES. */
int create_program_from_resource(shader_program_t *t_program, uint32_t t_prog_id);

/** @private Deletes a shader program in {@param t_program} based on {@param t_prog_id} from SHADER_PROGRAM_RESOURCES. */
int delete_program_from_resource(shader_program_t *t_program, uint32_t t_prog_id);

#endif //NM_SHADER_MANAGER_H
