<?php

class nci__short_tandem_repeat_formatter extends ChadoFieldFormatter
{
    // The default label for this field.
    public static $default_label = 'SSR';

    // The list of field types for which this formatter is appropriate.
    public static $field_types = ['nci__short_tandem_repeat'];

    /**
     *
     * @see TripalFieldFormatter::view()
     */
    public function view(&$element, $entity_type, $entity, $langcode, $items, $display)
    {
        // Get the settings
        $settings = $display['settings'];
    }
}
