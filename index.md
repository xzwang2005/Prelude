---
layout: default
title: Prelude
---
It is said that programming is art. In reality, it is often a loop of three steps: search, copy and paste, thanks to google and ever growing number of libraries.

Chromium is arguably the Mother of All Open-source Libraries. Most likely, any functionality an application ever need already has an implementation somewhere in Chromium.

However, reuse Chromium code can be challenging. The problem is usually not the lack of, but rather too much information, accompanied by very few hands-on samples and abuse of acronyms.

Therefore, Prelude is aimed to be a cookbook for Chromium. It sets up a playground with a subset of Chromium code, and provides recipes on how to use those frameworks.

Due to author's lack of perspective, i.e., plain ignorance, examples provided here may have errors, comments may be utterly wrong. So any feedback is welcome. Better yet, please feel free to write your own examples and add it to Prelude. You will get full credit for your contribution.

<div class="content">
<h2>Table of Contents</h2>
{% for item in site.data.navigate.toc %}
  <h3>{{ item.title }}</h3>
    <ul>
      {% for entry in item.subfolderitems %}
        <li><a href="{{ site.baseurl }}{{ entry.url }}">{{ entry.page }}</a></li>
      {% endfor %}
    </ul>
{% endfor %}
</div>