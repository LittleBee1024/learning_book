const RootComponent = {
   data() {
      return {
         message: '',
         check: false,
         checkedNames: [],
         picked: '',
         selected: '',
         options: [
            { text: 'One', value: 'A' },
            { text: 'Two', value: 'B' },
            { text: 'Three', value: 'C' }
         ]
      }
   }
}

const vm = Vue.createApp(RootComponent).mount('#app')
