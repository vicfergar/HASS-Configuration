import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import fan, duty_cycle
from esphome.const import CONF_OUTPUT_ID
from .. import rowenta_fan_ns

AUTO_LOAD = ['sensor','duty_cycle']

RowentaFan = rowenta_fan_ns.class_('RowentaFan', cg.Component)

CONFIG_SCHEMA = fan.FAN_SCHEMA.extend({
    cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(RowentaFan),
}).extend(cv.COMPONENT_SCHEMA)


def to_code(config):
    state = yield fan.create_fan_state(config)
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID], state)
    yield cg.register_component(var, config)