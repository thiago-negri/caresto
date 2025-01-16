#pragma once

#include <caresto/data/cds_systems.h>

cds_body_id csb_add(struct cds_systems *systems, struct cds_body *body);

struct cds_body *csb_get(struct cds_systems *systems, cds_body_id id);

void csb_remove(struct cds_systems *systems, cds_body_id id);

bool csb_grounded(struct cds_systems *systems, cds_body_id id);

void csb_tick(struct cds_systems *systems);
