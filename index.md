---
layout: default
title: Prelude
---
It is said that programming is art. In reality, it is often a loop of three steps: search, copy and paste, thanks to google and the ever-growing number of libraries.

Chromium is arguably the Mother of All Open-source Libraries. Very likely, any functionalities an application ever need already have implementations in Chromium.

However, reuse Chromium code is challenging. The problem is often not the lack of, but rather too much information, accompanied by very few hands-on samples and omnipresent acronyms.

Prelude is aimed to help you get comfortable with Chromium source. It picks a subset of Chromium code and shows how to use it.

Disclaimer: code provided here may have errors, comments may be utterly wrong. Therefore any feedback is welcome. Better yet, please feel free to write your own examples and add them to Prelude. You will get full credit for your contribution.

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