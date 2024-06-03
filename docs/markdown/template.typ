// Some definitions presupposed by pandoc's typst output.
#let horizontalrule = [
  #line(start: (25%,0%), end: (75%,0%))
]

#let endnote(num, contents) = [
  #stack(dir: ltr, spacing: 3pt, super[#num], contents)
]

#let conf(
  title: none, authors: none, keywords: none, date: datetime.today(),
  lang: none, region: "CH", abstract: none, margin: none,
  paper: none, font: "Default", fontsize: 1em, sectionnumbering: none,
  cols: 1, body
) = {
  
  show heading: set block(above: 0.7em, below: 0.7em)
  show link: underline
  set heading(outlined: false)
  set text(
    font: "Default",
    size: fontsize
  )

  if title != none [
    #line(length: 100%)
    #align(center)[
      = #title

      #if authors != none [
        #pad(
          top: 0.5em,
          bottom: 0.5em,
          x: 2em,
          grid(
            columns: (1fr,) * calc.min(3, authors.len()),
            gutter: 1em,
            ..authors.map(author => align(center)[
              #stack(
                dir: ttb,
                spacing: 0.5em,                
                strong(author.name),
                if author.affiliation != none [
                  #emph(author.affiliation)
                ],
                if author.email != none [
                  #author.email.replace("\@", "@")
                ]
              )
              
            ]),
          ),
        )
      ]
    ]
    #line(length: 100%)
  ]

  if abstract != none [
    #abstract
  ]

  set heading(
    numbering: sectionnumbering,
    outlined: true
  )
  show heading.where(level: 1): it => [
    #pagebreak()
    #it
  ]

  counter(heading).update(0)

  set outline(indent: true)

  body
}
