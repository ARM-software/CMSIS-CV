set(EXPORTED 
{% for t in tests -%}
  _{{t}}
{% endfor -%}
    )

