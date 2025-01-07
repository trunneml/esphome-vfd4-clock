import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import display
from esphome.const import (
    CONF_CLK_PIN,
    CONF_DATA_PIN,
    CONF_STB_PIN,
    CONF_ID,
    CONF_LAMBDA,
    CONF_INTENSITY,
)

CODEOWNERS = ["@trunneml"]

vfd4_ns = cg.esphome_ns.namespace("vfd4")
VFD4Display = vfd4_ns.class_("VFD4Display", cg.PollingComponent)
VFD4DisplayRef = VFD4Display.operator("ref")

CONFIG_SCHEMA = display.BASIC_DISPLAY_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(VFD4Display),
        cv.Optional(CONF_INTENSITY, default=7): cv.All(
            cv.uint8_t, cv.Range(min=0, max=7)
        ),
        cv.Required(CONF_CLK_PIN): pins.gpio_output_pin_schema,
        cv.Required(CONF_DATA_PIN): pins.gpio_output_pin_schema,
        cv.Required(CONF_STB_PIN): pins.gpio_output_pin_schema,
    }
).extend(cv.polling_component_schema("1s"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await display.register_display(var, config)

    clk = await cg.gpio_pin_expression(config[CONF_CLK_PIN])
    cg.add(var.set_clk_pin(clk))
    data = await cg.gpio_pin_expression(config[CONF_DATA_PIN])
    cg.add(var.set_data_pin(data))
    stb = await cg.gpio_pin_expression(config[CONF_STB_PIN])
    cg.add(var.set_stb_pin(stb))

    cg.add(var.set_intensity(config[CONF_INTENSITY]))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(VFD4DisplayRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
