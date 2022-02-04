const TodoButton = {
   props: ['action'],
   template: `
      <div>Prop: {{action}}</div>
      <button>
         <slot></slot>
      </button>
   `
}

const RootComponent = {
   data() {
      return {
         message: 'text'
      }
   },
   components: {
      'todo-button': TodoButton
   }
}

const vm = Vue.createApp(RootComponent).mount('#app')
