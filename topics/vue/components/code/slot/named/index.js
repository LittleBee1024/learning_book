const NamedSlot = {
   template: `
      <div>
         <header>
            <slot name="header">Default Header</slot>
         </header>
         <main>
            <slot>Default Main</slot>
         </main>
         <footer>
            <slot name="footer">Default Footer</slot>
         </footer>
      </div>
   `
}

const RootComponent = {
   components: {
      'named-slot': NamedSlot
   }
}

const vm = Vue.createApp(RootComponent).mount('#app')
