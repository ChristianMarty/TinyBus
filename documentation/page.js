class PageA4 extends HTMLElement {
    
    static observedAttributes = ["barcode"];

    // Static properties shared by all instances
    static pageNumberCounter = 0;
    static barcode = undefined;

    static pageNumber() {
        this.pageNumberCounter += 1;
        return this.pageNumberCounter;
    }

    // connect component
    connectedCallback() {
        if(PageA4.barcode===undefined && this.hasAttribute("barcode")){
            PageA4.barcode = this.getAttribute("barcode");
        } 
        const shadow = this.attachShadow({ mode: 'closed' });
        let currentPage = PageA4.pageNumber();
        window.setTimeout(() => {
            let barcode = PageA4.barcode;
            let totalPages = PageA4.pageNumberCounter;
            const header = `
            <link rel="stylesheet" href="documentTemplate_A4.css">
            <link rel="stylesheet" href="pageLayout.css">
            <link rel="stylesheet" href="global.css">
        
            <div class='page'>
            <div class='content'>
            <div class='header'>
                <div class='header_left'><h1 class='header'>TinyBus</h1></div>
                <div class='header_center'>
                    <h2 class='header_center'>${ barcode }</h2>
                    <p class='header_center'>Page ${ currentPage  } of ${ totalPages }</p>
                </div>
                <div class='header_right'><img class='header' src='logo.png' alt='logo'></div>
            </div>
            <div class='main'>
            `;
        
            const footer = `
            </div>
                <div class='footer'>
                    <p class="footer"><b>christian-marty.ch</b></p>
                </div>
            </div> 
            </div>
            `;
            shadow.innerHTML = header+this.innerHTML+footer;
        }, 0);
    }
  }

customElements.define('document-page', PageA4);
