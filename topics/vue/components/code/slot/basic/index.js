const TodoButton = {
   props: {
      action: {
         type: String,
         default: 'Add'
      }
   },
   template: `
      <button><slot>{{ action }} an item</slot></button>
   `
}

const RootComponent = {
   data() {
      return {
         item: 'First Item'
      }
   },
   components: {
      'todo-button': TodoButton
   }
}

const vm = Vue.createApp(RootComponent).mount('#app')
