vacuum_entity_id = data.get("entity_id")
input_boolean_entities = data.get("input_boolean_entities")

input_boolean_entities = {k:v for element in input_boolean_entities for k,v in element.items()}
segments = []

for entity_id, segment in input_boolean_entities.items():
    state = hass.states.get(entity_id)
    # filter out input_boolean not on
    if (state.state == 'on'):
        segments.append(segment)

if not segments:
    logger.error("No segment is active")
else:
    logger.info("Cleaning segments: %s", segments)
    service_data = {"entity_id": vacuum_entity_id, "segments": segments}
    hass.services.call("xiaomi_miio", "vacuum_clean_segment", service_data, False)
