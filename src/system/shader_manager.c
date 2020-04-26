// nolmoonen v1.0.0
#include <stdlib.h>
#include "shader_manager.h"
#include <util/log.h>

int initialize_shader_manager()
{
    create_clist(
            &shader_list, SHADER_COUNT, sizeof(shader_program_t),
            (int (*)(void *, uint32_t)) create_program_from_resource,
            (int (*)(void *, uint32_t)) delete_program_from_resource
    );

    return EXIT_SUCCESS;
}

int cleanup_shader_manager()
{
    delete_clist(&shader_list);

    return EXIT_SUCCESS;
}

int create_program_from_resource(shader_program_t *t_program, uint32_t t_prog_id)
{
    // find index of program shader
    int32_t prog_index = -1;
    for (uint32_t i = 0; i < sizeof(SHADER_PROGRAM_RESOURCES) / sizeof(shader_res_t); i++) {
        if (SHADER_PROGRAM_RESOURCES[i].m_id == t_prog_id) {
            prog_index = i;
            break;
        }
    }
    if (prog_index == -1) {
        nm_log(LOG_ERROR, "\"%d\" is not a registered shader program id\n", t_prog_id);

        return EXIT_FAILURE;
    }

    if (create_shader_program(
            t_program,
            SHADER_PROGRAM_RESOURCES[prog_index].m_vert_text, *SHADER_PROGRAM_RESOURCES[prog_index].m_vert_len,
            SHADER_PROGRAM_RESOURCES[prog_index].m_frag_text, *SHADER_PROGRAM_RESOURCES[prog_index].m_frag_len
    ) == EXIT_FAILURE) {
        nm_log(LOG_ERROR, "failed to create shader program\n");

        return EXIT_FAILURE;
    }

    nm_log(LOG_INFO, "created shader program with id \"%d\"\n", t_prog_id);

    return EXIT_SUCCESS;
}

int delete_program_from_resource(shader_program_t *t_program, uint32_t t_prog_id)
{
    delete_shader_program(t_program);

    nm_log(LOG_INFO, "deleted shader program with id \"%d\"\n", t_prog_id);

    return EXIT_SUCCESS;
}

int request_program(shader_program_t *t_program, uint32_t t_prog_id)
{
    if (get_clist(&shader_list, (void *) t_program, t_prog_id) == EXIT_FAILURE) {
        nm_log(LOG_ERROR, "could not request texture \"%d\"\n", t_prog_id);

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
